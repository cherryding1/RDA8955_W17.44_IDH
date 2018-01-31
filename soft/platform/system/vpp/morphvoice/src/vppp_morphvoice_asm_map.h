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

#define COMMON_GLOBAL_X_SIZE                                    296
#define COMMON_GLOBAL_Y_SIZE                                    12
#define HI_GLOBAL_X_SIZE                                        4238
#define HI_GLOBAL_Y_SIZE                                        0
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0
#define HI_LOCAL_X_SIZE                                         0
#define HI_LOCAL_Y_SIZE                                         0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define HI_GLOBAL_X_BEGIN_ADDR                                  (296 + RAM_X_BEGIN_ADDR)
#define HI_GLOBAL_Y_BEGIN_ADDR                                  (12 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (4534 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (12 + RAM_Y_BEGIN_ADDR)
#define HI_LOCAL_X_BEGIN_ADDR                                   (4534 + RAM_X_BEGIN_ADDR)
#define HI_LOCAL_Y_BEGIN_ADDR                                   (12 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define VPP_HELLO_MODE_NO                                   (-1) //alias
#define VPP_HELLO_MODE_HI                                   (0) //alias
#define VPP_HELLO_MODE_BYE                                  (1) //alias
#define VPP_HELLO_ERROR_NO                                  (0) //alias
#define VPP_HELLO_ERROR_YES                                 (-1) //alias
#define VPP_HELLO_SAYS_HI                                   (0x0111) //alias
#define VPP_HELLO_SAYS_BYE                                  (0x0B1E) //alias
//VPP_HELLO_CONST_STRUCT
#define VPP_HELLO_CONST_STRUCT                              (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //struct
#define VPP_HELLO_CONST_SAYS_HI                             (0 + VPP_HELLO_CONST_STRUCT) //1 word
#define VPP_HELLO_CONST_SAYS_BYE                            (2 + VPP_HELLO_CONST_STRUCT) //1 word

#define COS_TABLE_ADDR                                      (4 + COMMON_GLOBAL_X_BEGIN_ADDR) //129 short
#define TAN_TABLE_ADDR                                      (133 + COMMON_GLOBAL_X_BEGIN_ADDR) //33 short
#define ingSumPhase_ADDR                                    (166 + COMMON_GLOBAL_X_BEGIN_ADDR) //129 short
#define gRover_ADDR                                         (295 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short

//GLOBAL_VARS
//VPP_HELLO_CFG_STRUCT
#define VPP_HELLO_CFG_STRUCT                                (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_MODE                                         (0 + VPP_HELLO_CFG_STRUCT) //1 short
#define GLOBAL_RESET                                        (1 + VPP_HELLO_CFG_STRUCT) //1 short
#define PITCH_SHIFT_ADDR                                    (2 + VPP_HELLO_CFG_STRUCT) //1 short
#define RESEVER10_ADDR                                      (3 + VPP_HELLO_CFG_STRUCT) //1 short

//VPP_HELLO_STATUS_STRUCT
#define VPP_HELLO_STATUS_STRUCT                             (4 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_USED_MODE                                    (0 + VPP_HELLO_STATUS_STRUCT) //1 short
#define GLOBAL_ERR_STATUS                                   (1 + VPP_HELLO_STATUS_STRUCT) //1 short
#define GLOBAL_HELLO_STATUS                                 (2 + VPP_HELLO_STATUS_STRUCT) //1 word

//VPP_HELLO_CODE_CFG_STRUCT
#define VPP_HELLO_CODE_CFG_STRUCT                           (8 + COMMON_GLOBAL_Y_BEGIN_ADDR) //struct
#define GLOBAL_HI_CODE_PTR                                  (0 + VPP_HELLO_CODE_CFG_STRUCT) //1 word
#define GLOBAL_CONST_PTR                                    (2 + VPP_HELLO_CODE_CFG_STRUCT) //1 word


/*****************
 **  HI_GLOBAL  **
 *****************/

//GLOBAL_VARS
#define gInFIFO_ADDR                                        (0 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define gOutFIFO_ADDR                                       (256 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingLastPhase_ADDR                                   (512 + HI_GLOBAL_X_BEGIN_ADDR) //130 short
#define ingOutputAccum_ADDR                                 (642 + HI_GLOBAL_X_BEGIN_ADDR) //512 short
#define FFT_INPUT_ADDRESS                                   (1154 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define FFT_OUTPUT_ADDRESS                                  (1410 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingFFTworksp_ADDRESS                                (1666 + HI_GLOBAL_X_BEGIN_ADDR) //1024 short
#define sign_ADDRESS                                        (2690 + HI_GLOBAL_X_BEGIN_ADDR) //2 short
#define TEMP_UR_ADDRESS                                     (2692 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_UI_ADDRESS                                     (2693 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_LE2_ADDRESS                                    (2694 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_LE_ADDRESS                                     (2695 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define WR_ADDRESS                                          (2696 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define WI_ADDRESS                                          (2697 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP_ADDR_ADDRESS                                   (2698 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define TEMP1_ADDR_ADDRESS                                  (2699 + HI_GLOBAL_X_BEGIN_ADDR) //1 short
#define ingAnaMagn_ADDRESS                                  (2700 + HI_GLOBAL_X_BEGIN_ADDR) //512 short
#define ingAnaFreq_ADDRESS                                  (3212 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingSynFreq_ADDR                                     (3468 + HI_GLOBAL_X_BEGIN_ADDR) //256 short
#define ingSynMagn_ADDRESS                                  (3724 + HI_GLOBAL_X_BEGIN_ADDR) //512 short
#define GINOUT_ADDR_ADDRESS                                 (4236 + HI_GLOBAL_X_BEGIN_ADDR) //2 short

/********************
 **  COMMON_LOCAL  **
 ********************/

/****************
 **  HI_LOCAL  **
 ****************/

//RAM_X: size 0x2800, used 0x11b6   RAM_Y: size 0x2800, used 0x000c

#endif
