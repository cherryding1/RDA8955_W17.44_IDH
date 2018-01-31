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

#define ANALOGTV_GLOBAL_X_SIZE                                  9966
#define ANALOGTV_GLOBAL_Y_SIZE                                  7664
#define ANALOGTV_LOCAL_X_SIZE                                   0
#define ANALOGTV_LOCAL_Y_SIZE                                   0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define ANALOGTV_GLOBAL_X_BEGIN_ADDR                            (0 + RAM_X_BEGIN_ADDR)
#define ANALOGTV_GLOBAL_Y_BEGIN_ADDR                            (0 + RAM_Y_BEGIN_ADDR)
#define ANALOGTV_LOCAL_X_BEGIN_ADDR                             (9966 + RAM_X_BEGIN_ADDR)
#define ANALOGTV_LOCAL_Y_BEGIN_ADDR                             (7664 + RAM_Y_BEGIN_ADDR)

/***********************
 **  ANALOGTV_GLOBAL  **
 ***********************/

//GLOBAL_VARS
//GLOBAL_CONST_X_STRUCT
#define GLOBAL_CONST_X_STRUCT                               (0 + ANALOGTV_GLOBAL_X_BEGIN_ADDR)
#define CONST_TabV2R_ADDR                                   (0 + GLOBAL_CONST_X_STRUCT) //256 short
#define CONST_TableV2G_ADDR                                 (256 + GLOBAL_CONST_X_STRUCT) //256 short
#define CONST_1_ADDR                                        (CONST_TableV2G_ADDR+2)
#define CONST_R_ARRAY                                       (512 + GLOBAL_CONST_X_STRUCT) //616 short
#define CONST_B_ARRAY                                       (1128 + GLOBAL_CONST_X_STRUCT) //512 short
#define CONST_B128_ARRAY                                    (CONST_B_ARRAY+256)
#define CONST_R128_ARRAY                                    (CONST_R_ARRAY+256)

#define CONST_R_ADDR                                        (1640 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_G_ADDR                                        (1642 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //2 short
#define CONST_B_ADDR                                        (1644 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //2 short
#define GLOBAL_TEMP_BUF_ADDR                                (1646 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //1440 short
#define GLOBAL_TEMP_PTMP2_ADDR                              (GLOBAL_TEMP_BUF_ADDR+720)
#define GLOBAL_BUF_IN_ADDR                                  (3086 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //6400 short
#define GLOBAL_BUF_OUT_ADDR                                 (9486 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //240 short
#define GLOBAL_BUF_OUT_ADDR2                                (9726 + ANALOGTV_GLOBAL_X_BEGIN_ADDR) //240 short

//GLOBAL_VARS
//GLOBAL_INPUT_STRUCT
#define GLOBAL_INPUT_STRUCT                                 (0 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR)
#define INPUT_ADDR_ADDR                                     (0 + GLOBAL_INPUT_STRUCT) //2 short
#define OUTPUT_ADDR_ADDR                                    (2 + GLOBAL_INPUT_STRUCT) //2 short
#define BLEND_ADDR_ADDR                                     (4 + GLOBAL_INPUT_STRUCT) //2 short
#define INPUT_SRC_W_ADDR                                    (6 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_SRC_H_ADDR                                    (7 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_CUT_W_ADDR                                    (8 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_CUT_H_ADDR                                    (9 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_DEC_W_ADDR                                    (10 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_DEC_H_ADDR                                    (11 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_RESET_ADDR                                    (12 + GLOBAL_INPUT_STRUCT) //1 short
#define INPUT_ROTATE_ADDR                                   (13 + GLOBAL_INPUT_STRUCT) //1 short

//GLOBAL_CONST_Y_STRUCT
#define GLOBAL_CONST_Y_STRUCT                               (14 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR)
#define CONST_TabU2G_ADDR                                   (0 + GLOBAL_CONST_Y_STRUCT) //256 short
#define CONST_TabU2B_ADDR                                   (256 + GLOBAL_CONST_Y_STRUCT) //256 short
#define CONST_G_ARRAY                                       (512 + GLOBAL_CONST_Y_STRUCT) //512 short
#define CONST_G128_ARRAY                                    (CONST_G_ARRAY+256)

#define INPUT_IN_MODE_ADDR                                  (1038 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BLEND_DMASIZE_ADDR                           (1039 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_BLEND_DMAINC_ADDR                            (1040 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_DEAD_UX_ADDR                                 (1042 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_TEMP_BUF2_ADDR                               (1043 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_NUM_ADDR                              (1044 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_COUNT_ADDR                            (1045 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DEAD_OUT_LINE_ADDR                           (1046 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_INLINE_NUM_1_ADDR                            (1047 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM1_ADDR                                    (1048 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_NUM3_ADDR                                    (1049 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_P1_ADDR                                      (1050 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_P2_ADDR                                      (1051 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_DMAIN_SIZE_ADDR                              (1052 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEX_SIZE_ADDR                             (1054 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_SCALEY_SIZE_ADDR                             (1056 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_MASK_ADDR                                    (1058 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define CONST_0XFF00FF_ADDR                                 (1060 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //2 short
#define GLOBAL_FLAG_WRITE_ADDR                              (1062 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_FLAG_BLEND_ADDR                              (1063 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //1 short
#define GLOBAL_ROTATE_BUF_OUT_ADDR                          (1064 + ANALOGTV_GLOBAL_Y_BEGIN_ADDR) //6600 short
#define GLOBAL_TEMP_PTMP3_ADDR                              (GLOBAL_ROTATE_BUF_OUT_ADDR)

/**********************
 **  ANALOGTV_LOCAL  **
 **********************/

//RAM_X: size 0x2800, used 0x26ee   RAM_Y: size 0x2800, used 0x1df0

#endif
