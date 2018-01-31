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

#define COMMON_GLOBAL_X_SIZE                                    7382
#define COMMON_GLOBAL_Y_SIZE                                    10240
#define COMMON_LOCAL_X_SIZE                                     0
#define COMMON_LOCAL_Y_SIZE                                     0

/**************************
 **  SECTION_BEGIN_ADDR  **
 **************************/

#define COMMON_GLOBAL_X_BEGIN_ADDR                              (0 + RAM_X_BEGIN_ADDR)
#define COMMON_GLOBAL_Y_BEGIN_ADDR                              (0 + RAM_Y_BEGIN_ADDR)
#define COMMON_LOCAL_X_BEGIN_ADDR                               (7382 + RAM_X_BEGIN_ADDR)
#define COMMON_LOCAL_Y_BEGIN_ADDR                               (10240 + RAM_Y_BEGIN_ADDR)

/*********************
 **  COMMON_GLOBAL  **
 *********************/

//GLOBAL_VARS
#define _ps_stack__main_RAM_X_0                             (8192)
#define _ps_stack_voc_dec_main_RAM_X_0                      (8194)
#define _ps_stack___vpp_gifdec_main_GDI_IMAGE_GIF_CODEC_VOC_RAM_X_0         (8198)
#define _ps_stack_init_cache_RAM_X_0                        (8198)
#define _ps_stack_cache_flush_RAM_X_0                       (8198)
#define _ps_stack_cache_create_RAM_X_0                      (8200)
#define _ps_stack_gdi_non_resizer_put_voc_16_RAM_X_0            (8238)
#define _ps_stack_gdi_resizer_put_voc_16_RAM_X_0            (8238)
#define _ps_stack_gdi_resizer_set_want_sy_RAM_X_0           (8238)
#define _ps_stack_gdi_resizer_init_voc_RAM_X_0              (8238)
#define _ps_stack_gdi_resizer_set_want_sy_get_dy_RAM_X_0            (8240)
#define _ps_stack_gdi_resizer_set_want_sy_dest_RAM_X_0          (8240)
#define _ps_stack_cache_access_RAM_X_0                      (8242)
#define _ps_stack___cache_voc_link_htab_ent_RAM_X_0         (8284)
#define _ps_stack___cache_voc_unlink_htab_ent_RAM_X_0           (8284)
#define _ps_stack___cache_voc_update_way_list_RAM_X_0           (8284)
#define _ps_stack_cp_blk_access_fn_RAM_X_0                  (8284)
#define CACHE_DATA                                          (0 + COMMON_GLOBAL_X_BEGIN_ADDR) //5120 short
#define CACHE_HASH                                          (5120 + COMMON_GLOBAL_X_BEGIN_ADDR) //32 short
//CACHE_STRUCT
#define CACHE_STRUCT                                        (5152 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define CACHE_STRUCT_NAME                                   (0 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_NBSETS                                 (1 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_BSIZE                                  (2 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_BALLOC                                 (3 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_USIZE                                  (4 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_ASSOC                                  (5 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_POLICY                                 (6 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_HIT_LATENCY                            (7 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_BLK_ACCESS_FN                          (8 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_HSIZE                                  (9 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_BLK_MASK                               (10 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_SET_SHIFT                              (12 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_SET_MASK                               (14 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_TAG_SHIFT                              (16 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_TAG_MASK                               (18 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_TAGSET_MASK                            (20 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_BUS_FREE                               (22 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_HITS                                   (23 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_MISSES                                 (24 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_REPLACEMENTS                           (25 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_WTITEBACKS                             (26 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_INVALIDATIONS                          (27 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_LAST_TAGSET                            (28 + CACHE_STRUCT) //2 short
#define CACHE_STRUCT_LAST_BLK                               (30 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_DATA                                   (31 + CACHE_STRUCT) //1 short
#define CACHE_STRUCT_SET                                    (32 + CACHE_STRUCT) //16 short

#define CACHE_STRUCT_PTR                                    (5200 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define CACHE_SRC_ADDR                                      (5202 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define CACHE_DST_ADDR                                      (5204 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define CACHE_SRC_VALUE                                     (5206 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define CACHE_DST_VALUE                                     (5208 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define CACHE_DUMMY                                         (5210 + COMMON_GLOBAL_X_BEGIN_ADDR) //6 short
//GIF_CFG_STRUCT
#define GIF_CFG_STRUCT                                      (5216 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define GIF_CONFIG_GCT                                      (0 + GIF_CFG_STRUCT) //256 short
#define GIF_CONFIG_SHADOW_ADRS                              (256 + GIF_CFG_STRUCT) //2 short
#define GIF_CONFIG_SHADOW_WIDTH                             (258 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_SHADOW_HEIGHT                            (259 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_IMG_ADRS                                 (260 + GIF_CFG_STRUCT) //2 short
#define GIF_CONFIG_IMG_SIZE                                 (262 + GIF_CFG_STRUCT) //2 short
#define GIF_CONFIG_WIDTH                                    (264 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_HEIGHT                                   (265 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_X                                        (266 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_Y                                        (267 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_TRANSPARENT_ENABLE                       (268 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_TRANSPARENT_INDEX                        (269 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_IS_INTERLACE                             (270 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_TEMP                                     (271 + GIF_CFG_STRUCT) //1 short
#define GIF_CONFIG_BUFFER_OFFSET                            (272 + GIF_CFG_STRUCT) //2 short
#define GIF_CONFIG_RESIZER                                  (274 + GIF_CFG_STRUCT) //2 short
#define GIF_CFG_DUMMY                                       (276 + GIF_CFG_STRUCT) //4 short

//GIF_ST_STRUCT
#define GIF_ST_STRUCT                                       (5496 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define ERROR_CODE                                          (0 + GIF_ST_STRUCT) //1 short
#define DEC_FINISHED                                        (1 + GIF_ST_STRUCT) //1 short
#define GIF_ST_DUMMY                                        (2 + GIF_ST_STRUCT) //6 short

//GDI_RESIZER_STRUCT
#define GDI_RESIZER_STRUCT                                  (5504 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define GDI_RESIZER_DEST                                    (0 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_DEST_PITCH_JUMP                         (2 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_WANT_SX_TABLE_EXT                       (4 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_WANT_SX_TABLE_END_EXT                   (6 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_NEXT_WANT_SX_EXT                        (8 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_WANT_SY_TABLE_EXT                       (10 + GDI_RESIZER_STRUCT) //2 short
#define GDI_RESIZER_WANT_SX_TABLE_SIZE                      (12 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SY_TABLE_SIZE                      (13 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_BITSPERPIXSELS                          (14 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_SRC_HEIGHT_RANGE                        (15 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_DEST_HEIGHT_RANGE                       (16 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_OFFSET_DX                               (17 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_OFFSET_DY                               (18 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_DX1                                (19 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_DX2                                (20 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_DY1                                (21 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_DY2                                (22 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SX                                 (23 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SY                                 (24 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_NEXT_WANT_SY                            (25 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_DIR_X                                   (26 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_DIR_Y                                   (27 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_START_SX                           (28 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_START_SY                           (29 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_END_SX                             (30 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_END_SY                             (31 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SX_TABLE                           (32 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SX_TABLE_END                       (33 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_NEXT_WANT_SX                            (34 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SY_TABLE                           (35 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_WANT_SY_TABLE_END                       (36 + GDI_RESIZER_STRUCT) //1 short
#define GDI_RESIZER_DUMMY                                   (37 + GDI_RESIZER_STRUCT) //3 short

#define __gdi_primitive_voc_gdi_resizer_sx_buffer_voc           (5544 + COMMON_GLOBAL_X_BEGIN_ADDR) //640 short
#define __gdi_primitive_voc_gdi_resizer_sy_buffer_voc           (6184 + COMMON_GLOBAL_X_BEGIN_ADDR) //640 short
#define __vpp_gifdec_main_GDI_GIF_STACK1                    (6824 + COMMON_GLOBAL_X_BEGIN_ADDR) //288 short
#define __vpp_gifdec_main_GIF_SRC                           (7112 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define __vpp_gifdec_main_GIF_SRC_BEGIN                     (7114 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define __vpp_gifdec_main_GIF_SRC_END                       (7116 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define __vpp_gifdec_main_GIF_SRC_SIZE                      (7118 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define __vpp_gifdec_main_CURRENT_PALETTE                   (7120 + COMMON_GLOBAL_X_BEGIN_ADDR) //2 short
#define __vpp_gifdec_main_GDI_IMAGE_GIF_WIDTH               (7122 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
#define __vpp_gifdec_main_GDI_IMAGE_GIF_HEIGHT              (7123 + COMMON_GLOBAL_X_BEGIN_ADDR) //1 short
//CACHE_PROFILE
#define CACHE_PROFILE                                       (7124 + COMMON_GLOBAL_X_BEGIN_ADDR)
#define CUR_POS                                             (0 + CACHE_PROFILE) //1 short
#define CACHE_DEBUG                                         (1 + CACHE_PROFILE) //256 short


//GLOBAL_VARS
#define __vpp_gifdec_main_GDI_TREE_BUFFER1                  (0 + COMMON_GLOBAL_Y_BEGIN_ADDR) //10240 short

/********************
 **  COMMON_LOCAL  **
 ********************/

//RAM_X: size 0x4000, used 0x1cd6   RAM_Y: size 0x4000, used 0x2800

#endif
