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



#ifndef _INTERPOLATE8X8_H_
#define _INTERPOLATE8X8_H_

#include "mem_transfer.h"
typedef void (INTERPOLATE8X8) (uint8_t * dst,uint8_t * src,
                               int32_t stride,
                               int32_t rounding);
typedef INTERPOLATE8X8 *INTERPOLATE8X8_PTR;

//static int32_t s32rc[4] = {0, 1, 1, 2};
extern void transfer16x16_copy_c(uint8_t *dst,uint8_t *ref,
                                 int32_t stride,int32_t rounding);
extern void interpolate16x16_halfpel_v_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
extern void interpolate16x16_halfpel_h_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
extern void interpolate16x16_halfpel_hv_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
extern void mp4_transfer8x8_copy_c(uint8_t *dst,uint8_t *ref,
                                   int32_t stride,int32_t rounding);
extern void mp4_interpolate8x8_halfpel_v_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
extern void mp4_interpolate8x8_halfpel_h_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
extern void mp4_interpolate8x8_halfpel_hv_c(uint8_t *dst,uint8_t *ref,
        int32_t stride,int32_t rounding);
//FILE *file_test;//test
//extern int macroblock_number;
/*
static void
interpolate8x8_switch(uint8_t * cur,
                      uint8_t * refn,
                      int32_t dx,
                      int dy,
                      int32_t stride,
                      int32_t rounding)
{
    uint8_t *refn_mbsrc;
    int32_t pred_type = ((dx&0x1) << 1) + (dy&0x1);
    static INTERPOLATE8X8_PTR interplolate8x8_array[4] = {mp4_transfer8x8_copy_c,mp4_interpolate8x8_halfpel_v_c,mp4_interpolate8x8_halfpel_h_c,mp4_interpolate8x8_halfpel_hv_c};
    refn_mbsrc = refn + (dy >> 1)*stride + (dx >> 1);
    interplolate8x8_array[pred_type]((uint8_t *)cur,refn_mbsrc,stride,s32rc[pred_type]-rounding);
}

static void
interpolate16x16_switch(uint8_t * cur,
                      uint8_t * refn,
                      int32_t dx,
                      int32_t dy,
                      int32_t stride,
                      int32_t rounding)
{
    uint8_t *refn_mbsrc;
    int32_t pred_type = ((dx&0x1) << 1) + (dy&0x1);
    static INTERPOLATE8X8_PTR interpolate16x16_array[4] = {transfer16x16_copy_c,interpolate16x16_halfpel_v_c,interpolate16x16_halfpel_h_c,interpolate16x16_halfpel_hv_c};
    refn_mbsrc = refn + (dy >> 1)*stride + (dx >> 1);
    interpolate16x16_array[pred_type]((uint8_t *)cur,refn_mbsrc,stride,s32rc[pred_type]-rounding);
}
*/
#endif

