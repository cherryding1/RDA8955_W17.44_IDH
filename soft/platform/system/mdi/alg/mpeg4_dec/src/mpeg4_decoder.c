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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#include "mcip_debug.h"
#endif
#ifdef MP4_3GP_SUPPORT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <cyg/kernel/kapi.h>

#ifdef BFRAMES_DEC_DEBUG
#define BFRAMES_DEC
#endif

#include "xvid.h"
#include "portab.h"
#include "mpeg4_global.h"

#include "decoder.h"
#include "mpeg4_bitstream.h"
#include "mbcoding.h"

#include "quant.h"
#include "idct.h"

#include "mem_transfer.h"
#include "interpolate8x8.h"
//#include "image/font.h"


#include "mbcoding.h"
#include "mbprediction.h"
//#include "utils/timer.h"


#include "image.h"
//#include "colorspace.h"
//#include "postprocessing.h"
#include "mem_align.h"

#include "xvid_hw.h"
//#include "../include/global_macros.h"
//#include "../include/lp_ct810reg.h"

#include "vpp_mpeg4_dec_asm.h"

#ifdef SHEEN_VC_DEBUG
#include "voc2_library.h"
#include "voc2_define.h"
//#include "voc2_simulator.h"
#include "cs_types.h"
#include "mpeg4dec.h"
#include "..\map_addr\MAP_ADDR.H"
#else
#include "hal_sys.h"
#include "mmc.h"
#include "mdi_alg_common.h"
#include "sxr_tls.h"
#include "vpp_audiojpeg_dec.h"
extern VocWorkStruct vocWorkState;
#endif

#ifdef MEDIA_VOCVID_SUPPORT
#define VOC_DO
#endif

extern uint16_t roundtab_76[16];
extern uint32_t roundtab_79[16];
extern uint16 scan_tables[3][64];
extern void mpeg4_simple_idct_put(uint8 *dest, int line_size, int16 *block);
extern void mpeg4_simple_idct_add(uint8 *dest, int line_size, int16 *block);
#ifdef RAM_DCT3D
extern REVERSE_EVENT *DCT3D;
#else
extern REVERSE_EVENT const DCT3D[2][4096];
#endif


extern uint8 mpeg4_lowres;

//static cyg_mutex_t vld_access_mutex;

#define DIV2ROUND(n)  (((n)>>1)|((n)&1))
#define DIV2(n)       ((n)>>1)
#define DIVUVMOV(n) (((n) >> 1) + roundtab_79[(n) & 0x3]) //

#if 0

const short const_number[64] =
{
    0x80,0,1108,3784,1568,565,2276,3406,
    2408,799,4017,181,0x40,0,4,32,
    0x2000,0,63,56,31,
};

const short dc_lum_tab_voc[8][2] =
{
    {0, 0}, {4, 3}, {3, 3}, {0, 3},
    {2, 2}, {2, 2}, {1, 2}, {1, 2},
};

uint16_t const max_level_voc[2][64] =
{
    /* intra = 1, last = 0 */
    {
        27, 10, 5, 4, 3, 3, 3, 3,
        2, 2, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    /* intra = 1, last = 1 */
    {
        8, 3, 2, 2, 2, 2, 2, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }
};
uint16_t const max_level_h263[2][64] =
{
    /* intra = 0, last = 0 */
    {
        12, 6, 4, 3, 3, 3, 3, 2,
        2, 2, 2, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    },
    /* intra = 0, last = 1 */
    {
        3, 2, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0
    }
};

uint16_t const max_run_voc[2][64] =
{
    /* intra = 1, last = 0 */
    {
        0, 14, 9, 7, 3, 2, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    },
    /* intra = 1, last = 1 */
    {
        0, 20, 6, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    }
};
const uint16_t MPEG_DC_SCALAR_MULT[2][32] =
{
    {
        0x0000, 0x4001, 0x4001, 0x4001, 0x4001, //0-4
        0x38e4, 0x38e4, 0x3334, 0x3334, 0x2e8c, //5-9
        0x2e8c, 0x2aab, 0x2aab, 0x2763, 0x2763, //10-14
        0x2493, 0x2493, 0x2223, 0x2223, 0x2001, //15-19
        0x2001, 0x1e1f, 0x1e1f, 0x1c72, 0x1c72, //20-24
        0x1af3, 0x199a, 0x1862, 0x1746, 0x1643, //25-29
        0x1556, 0x147b              //30-32
    },
    {
        0x0000, 0x4001, 0x4001, 0x4001, 0x4001, //0-4
        0x3334, 0x2aab, 0x2493, 0x2001, 0x1e1f, //5-9
        0x1c72, 0x1af3, 0x199a, 0x1862, 0x1746, //10-14
        0x1643, 0x1556, 0x147b, 0x13b2, 0x12f7, //15-19
        0x124a, 0x11a8, 0x1112, 0x1085, 0x1001, //20-24
        0x0f10, 0x0e39, 0x0d7a, 0x0ccd, 0x0c31, //25-29
        0x0ba3, 0x0b22
    }              //30-32
};

short const TMNMVtab0voc[14][2] =
{
    {3, 4}, {-3, 4}, {2, 3}, {2, 3}, {-2, 3}, {-2, 3}, {1, 2},
    {1, 2}, {1, 2}, {1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}
};
short const TMNMVtab1voc[96][2] =
{
    {12, 10}, {-12, 10}, {11, 10}, {-11, 10},
    {10, 9}, {10, 9}, {-10, 9}, {-10, 9},
    {9, 9}, {9, 9}, {-9, 9}, {-9, 9},
    {8, 9}, {8, 9}, {-8, 9}, {-8, 9},
    {7, 7}, {7, 7}, {7, 7}, {7, 7},
    {7, 7}, {7, 7}, {7, 7}, {7, 7},
    {-7, 7}, {-7, 7}, {-7, 7}, {-7, 7},
    {-7, 7}, {-7, 7}, {-7, 7}, {-7, 7},
    {6, 7}, {6, 7}, {6, 7}, {6, 7},
    {6, 7}, {6, 7}, {6, 7}, {6, 7},
    {-6, 7}, {-6, 7}, {-6, 7}, {-6, 7},
    {-6, 7}, {-6, 7}, {-6, 7}, {-6, 7},
    {5, 7}, {5, 7}, {5, 7}, {5, 7},
    {5, 7}, {5, 7}, {5, 7}, {5, 7},
    {-5, 7}, {-5, 7}, {-5, 7}, {-5, 7},
    {-5, 7}, {-5, 7}, {-5, 7}, {-5, 7},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6}
};

short const TMNMVtab2voc[124][2] =
{
    {32, 12}, {-32, 12}, {31, 12}, {-31, 12},
    {30, 11}, {30, 11}, {-30, 11}, {-30, 11},
    {29, 11}, {29, 11}, {-29, 11}, {-29, 11},
    {28, 11}, {28, 11}, {-28, 11}, {-28, 11},
    {27, 11}, {27, 11}, {-27, 11}, {-27, 11},
    {26, 11}, {26, 11}, {-26, 11}, {-26, 11},
    {25, 11}, {25, 11}, {-25, 11}, {-25, 11},
    {24, 10}, {24, 10}, {24, 10}, {24, 10},
    {-24, 10}, {-24, 10}, {-24, 10}, {-24, 10},
    {23, 10}, {23, 10}, {23, 10}, {23, 10},
    {-23, 10}, {-23, 10}, {-23, 10}, {-23, 10},
    {22, 10}, {22, 10}, {22, 10}, {22, 10},
    {-22, 10}, {-22, 10}, {-22, 10}, {-22, 10},
    {21, 10}, {21, 10}, {21, 10}, {21, 10},
    {-21, 10}, {-21, 10}, {-21, 10}, {-21, 10},
    {20, 10}, {20, 10}, {20, 10}, {20, 10},
    {-20, 10}, {-20, 10}, {-20, 10}, {-20, 10},
    {19, 10}, {19, 10}, {19, 10}, {19, 10},
    {-19, 10}, {-19, 10}, {-19, 10}, {-19, 10},
    {18, 10}, {18, 10}, {18, 10}, {18, 10},
    {-18, 10}, {-18, 10}, {-18, 10}, {-18, 10},
    {17, 10}, {17, 10}, {17, 10}, {17, 10},
    {-17, 10}, {-17, 10}, {-17, 10}, {-17, 10},
    {16, 10}, {16, 10}, {16, 10}, {16, 10},
    {-16, 10}, {-16, 10}, {-16, 10}, {-16, 10},
    {15, 10}, {15, 10}, {15, 10}, {15, 10},
    {-15, 10}, {-15, 10}, {-15, 10}, {-15, 10},
    {14, 10}, {14, 10}, {14, 10}, {14, 10},
    {-14, 10}, {-14, 10}, {-14, 10}, {-14, 10},
    {13, 10}, {13, 10}, {13, 10}, {13, 10},
    {-13, 10}, {-13, 10}, {-13, 10}, {-13, 10}
};
short const mcbpc_inter_table_voc[257][2] =
{
    {VLC_ERROR, 0}, {255, 9}, {52, 9}, {36, 9}, {20, 9}, {49, 9}, {35, 8}, {35, 8},
    {19, 8}, {19, 8}, {50, 8}, {50, 8}, {51, 7}, {51, 7}, {51, 7}, {51, 7},
    {34, 7}, {34, 7}, {34, 7}, {34, 7}, {18, 7}, {18, 7}, {18, 7}, {18, 7},
    {33, 7}, {33, 7}, {33, 7}, {33, 7}, {17, 7}, {17, 7}, {17, 7}, {17, 7},
    {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6},
    {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5},
    {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {0, 1}
};
short const cbpy_table_voc[64][2] =
{
    {-1, 0}, {-1, 0}, {6, 6},  {9, 6},  {8, 5},  {8, 5},  {4, 5},  {4, 5},
    {2, 5},  {2, 5},  {1, 5},  {1, 5},  {0, 4},  {0, 4},  {0, 4},  {0, 4},
    {12, 4}, {12, 4}, {12, 4}, {12, 4}, {10, 4}, {10, 4}, {10, 4}, {10, 4},
    {14, 4}, {14, 4}, {14, 4}, {14, 4}, {5, 4},  {5, 4},  {5, 4},  {5, 4},
    {13, 4}, {13, 4}, {13, 4}, {13, 4}, {3, 4},  {3, 4},  {3, 4},  {3, 4},
    {11, 4}, {11, 4}, {11, 4}, {11, 4}, {7, 4},  {7, 4},  {7, 4},  {7, 4},
    {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2},
    {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}
};
short const mcbpc_intra_table_voc[64][2] =
{
    {-1, 0}, {20, 6}, {36, 6}, {52, 6}, {4, 4},  {4, 4},  {4, 4},  {4, 4},
    {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3},
    {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3},
    {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1}
};

#endif
extern uint8_t MPEG_DC_SCALAR[2][32];
extern uint16_t MPEG_DC_SCALAR_MULT[2][32];
extern uint16_t iQuant_MULT[32];

static int32_t s32rc[4] = {0, 1, 1, 2};

//int dct0[4096],dct1[4096];
#ifndef VOC_DO

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
interpolate16x16_quarterpel(uint8_t * cur,
                            uint8_t * refn,
                            int32_t dx,
                            int32_t dy,
                            int32_t stride,
                            int32_t rounding)
{

    uint8_t *refn_mbsrc;
    int32_t pred_type = (dx&3) | ((dy&3)<<2);;
    refn_mbsrc = refn + (dy >> 2)*stride + (dx >> 2);

    switch(pred_type)
    {
        case 0:
            //transfer8x8_copy(dst, src, stride);
            //transfer8x8_copy(dst+8, src+8, stride);
            //transfer8x8_copy(dst+8*stride, src+8*stride, stride);
            //transfer8x8_copy(dst+8*stride+8, src+8*stride+8, stride);
            transfer16x16_copy_c((uint8_t *)cur, refn_mbsrc, stride, 0);
            break;
        case 1:
            //Ops->H_Pass_Avrg(dst, src, 16, stride, rounding);//xvid_H_Pass_Avrg_16_mmx:  H_PASS_16 1, 0
            mp4_qpel16_mc10_c(cur, refn_mbsrc, stride);
            break;
        case 2:
            //Ops->H_Pass(dst, src, 16, stride, rounding);
            mp4_qpel16_mc20_c(cur, refn_mbsrc, stride);
            break;
        case 3:
            //Ops->H_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel16_mc30_c(cur, refn_mbsrc, stride);
            break;
        case 4:
            //Ops->V_Pass_Avrg(dst, src, 16, stride, rounding);
            mp4_qpel16_mc01_c(cur, refn_mbsrc, stride);
            break;
        case 5:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc11_c(cur, refn_mbsrc, stride);
            break;
        case 6:
            //Ops->H_Pass(tmp, src,   17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc21_c(cur, refn_mbsrc, stride);
            break;
        case 7:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc31_c(cur, refn_mbsrc, stride);
            break;
        case 8:
            //Ops->V_Pass(dst, src, 16, stride, rounding);
            mp4_qpel16_mc02_c(cur, refn_mbsrc, stride);
            break;
        case 9:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc12_c(cur, refn_mbsrc, stride);
            break;
        case 10:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc22_c(cur, refn_mbsrc, stride);
            break;
        case 11:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc32_c(cur, refn_mbsrc, stride);
            break;
        case 12:
            //Ops->V_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel16_mc03_c(cur, refn_mbsrc, stride);
            break;
        case 13:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc13_c(cur, refn_mbsrc, stride);
            break;
        case 14:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up( dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc23_c(cur, refn_mbsrc, stride);
            break;
        case 15:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel16_mc33_c(cur, refn_mbsrc, stride);
            break;
    }
}

static void
interpolate8x8_quarterpel(uint8_t * cur,
                          uint8_t * refn,
                          int32_t dx,
                          int32_t dy,
                          int32_t stride,
                          int32_t rounding)
{

    uint8_t *refn_mbsrc;
    int32_t pred_type = (dx&3) | ((dy&3)<<2);
    refn_mbsrc = refn + (dy >> 2)*stride + (dx >> 2);

    switch(pred_type)
    {
        case 0:
            //transfer8x8_copy(dst, src, stride);
            //transfer8x8_copy(dst+8, src+8, stride);
            //transfer8x8_copy(dst+8*stride, src+8*stride, stride);
            //transfer8x8_copy(dst+8*stride+8, src+8*stride+8, stride);
            mp4_transfer8x8_copy_c((uint8_t *)cur, refn_mbsrc, stride, 0);
            break;
        case 1:
            //Ops->H_Pass_Avrg(dst, src, 16, stride, rounding);//xvid_H_Pass_Avrg_16_mmx:  H_PASS_16 1, 0
            mp4_qpel8_mc10_c(cur, refn_mbsrc, stride);
            break;
        case 2:
            //Ops->H_Pass(dst, src, 16, stride, rounding);
            mp4_qpel8_mc20_c(cur, refn_mbsrc, stride);
            break;
        case 3:
            //Ops->H_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel8_mc30_c(cur, refn_mbsrc, stride);
            break;
        case 4:
            //Ops->V_Pass_Avrg(dst, src, 16, stride, rounding);
            mp4_qpel8_mc01_c(cur, refn_mbsrc, stride);
            break;
        case 5:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc11_c(cur, refn_mbsrc, stride);
            break;
        case 6:
            //Ops->H_Pass(tmp, src,   17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc21_c(cur, refn_mbsrc, stride);
            break;
        case 7:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc31_c(cur, refn_mbsrc, stride);
            break;
        case 8:
            //Ops->V_Pass(dst, src, 16, stride, rounding);
            mp4_qpel8_mc02_c(cur, refn_mbsrc, stride);
            break;
        case 9:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc12_c(cur, refn_mbsrc, stride);
            break;
        case 10:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc22_c(cur, refn_mbsrc, stride);
            break;
        case 11:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc32_c(cur, refn_mbsrc, stride);
            break;
        case 12:
            //Ops->V_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel8_mc03_c(cur, refn_mbsrc, stride);
            break;
        case 13:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc13_c(cur, refn_mbsrc, stride);
            break;
        case 14:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up( dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc23_c(cur, refn_mbsrc, stride);
            break;
        case 15:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel8_mc33_c(cur, refn_mbsrc, stride);
            break;
    }
}
static void
interpolate8x8_add_quarterpel(uint8_t * cur,
                              uint8_t * refn,
                              int32_t dx,
                              int32_t dy,
                              int32_t stride,
                              int32_t rounding)
{

    uint8_t *refn_mbsrc;
    int32_t pred_type = (dx&3) | ((dy&3)<<2);;
    refn_mbsrc = refn + (dy >> 2)*stride + (dx >> 2);

    switch(pred_type)
    {
        case 0:
            //transfer8x8_copy(dst, src, stride);
            //transfer8x8_copy(dst+8, src+8, stride);
            //transfer8x8_copy(dst+8*stride, src+8*stride, stride);
            //transfer8x8_copy(dst+8*stride+8, src+8*stride+8, stride);
            mp4_transfer8x8_add_c((uint8_t *)cur, refn_mbsrc, stride, 1);
            break;
        case 1:
            //Ops->H_Pass_Avrg(dst, src, 16, stride, rounding);//xvid_H_Pass_Avrg_16_mmx:  H_PASS_16 1, 0
            mp4_qpel8_add_mc10_c(cur, refn_mbsrc, stride);
            break;
        case 2:
            //Ops->H_Pass(dst, src, 16, stride, rounding);
            mp4_qpel8_add_mc20_c(cur, refn_mbsrc, stride);
            break;
        case 3:
            //Ops->H_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel8_add_mc30_c(cur, refn_mbsrc, stride);
            break;
        case 4:
            //Ops->V_Pass_Avrg(dst, src, 16, stride, rounding);
            mp4_qpel8_add_mc01_c(cur, refn_mbsrc, stride);
            break;
        case 5:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc11_c(cur, refn_mbsrc, stride);
            break;
        case 6:
            //Ops->H_Pass(tmp, src,   17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc21_c(cur, refn_mbsrc, stride);
            break;
        case 7:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc31_c(cur, refn_mbsrc, stride);
            break;
        case 8:
            //Ops->V_Pass(dst, src, 16, stride, rounding);
            mp4_qpel8_add_mc02_c(cur, refn_mbsrc, stride);
            break;
        case 9:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc12_c(cur, refn_mbsrc, stride);
            break;
        case 10:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc22_c(cur, refn_mbsrc, stride);
            break;
        case 11:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc32_c(cur, refn_mbsrc, stride);
            break;
        case 12:
            //Ops->V_Pass_Avrg_Up(dst, src, 16, stride, rounding);
            mp4_qpel8_add_mc03_c(cur, refn_mbsrc, stride);
            break;
        case 13:
            //Ops->H_Pass_Avrg(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc13_c(cur, refn_mbsrc, stride);
            break;
        case 14:
            //Ops->H_Pass(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up( dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc23_c(cur, refn_mbsrc, stride);
            break;
        case 15:
            //Ops->H_Pass_Avrg_Up(tmp, src, 17, stride, rounding);
            //Ops->V_Pass_Avrg_Up(dst, tmp, 16, stride, rounding);
            mp4_qpel8_add_mc33_c(cur, refn_mbsrc, stride);
            break;
    }
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

static void
interpolate8x8_add_switch(uint8_t * cur,
                          uint8_t * refn,
                          int32_t dx,
                          int dy,
                          int32_t stride,
                          int32_t rounding)
{
    uint8_t *refn_mbsrc;
    int32_t pred_type = ((dx&0x1) << 1) + (dy&0x1);
    static INTERPOLATE8X8_PTR interplolate8x8_array[4] = {mp4_transfer8x8_add_c,mp4_interpolate8x8_halfpel_v_add_c,mp4_interpolate8x8_halfpel_h_add_c,mp4_interpolate8x8_halfpel_hv_add_c};
    refn_mbsrc = refn + (dy >> 1)*stride + (dx >> 1);
    interplolate8x8_array[pred_type]((uint8_t *)cur,refn_mbsrc,stride,s32rc[pred_type]-rounding);
}
#endif
#if 0 //move to xvid.c for overlay
static void BitstreamInit(Bitstream * const bs,
                          void *const bitstream,
                          uint32_t length)
{
    bs->tail = bs->start = (int8 *)bitstream;
    bs->buf = ((*bs->tail)<<24)|((*(bs->tail+1))<<16)|((*(bs->tail+2))<<8)|(*(bs->tail+3));
    bs->tail += 4;
    bs->pos = 32;
    bs->length = length;
}



//
////////////test by longyl
//FILE * file_test;
//int macroblock_number;
//uint32_t frame_number;
//////////////////////////////////
static int
decoder_resize(DECODER * dec)
{
    int temp_size;
    /* free existing */
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);

    image_null(&dec->cur);
    image_null(&dec->refn[0]);
//  image_null(&dec->tmp);


    xvid_free(dec->ac_pred_base);
    xvid_free(dec->mv_buffer);
    dec->ac_pred_base = NULL;
    dec->mv_buffer = NULL;
    xvid_free(dec->qscale);
    dec->qscale = NULL;
    /* realloc */
    dec->mb_width = (dec->width + 15) >> 4;
    dec->mb_height = (dec->height + 15) >> 4;

    dec->edged_width =  (short)((dec->mb_width << 4) + (EDGE_SIZE << 1));
    dec->edged_height = (short)((dec->mb_height << 4) + (EDGE_SIZE << 1));
    dec->stride[0] = dec->stride[1] = dec->stride[2] = dec->stride[3] = dec->edged_width;
    dec->stride[4] = dec->stride[5] = dec->edged_width >> 1;
    if (image_create(&dec->cur, dec->edged_width, dec->edged_height)
            || image_create(&dec->refn[0], dec->edged_width, dec->edged_height))
        //    || image_create(&dec->tmp, dec->edged_width, dec->edged_height)   )
    {
        goto memory_error;
    }

    /* nothing happens if that fails */
    temp_size = sizeof(int16_t) * (dec->mb_width + 2);
    dec->qscale =
        xvid_malloc(temp_size, CACHE_LINE);
    if (dec->qscale)
        memset(dec->qscale, 0, temp_size);
    else
    {
        goto memory_error;
    }
    /* change order, sheen
    dec->qscale_current = dec->qscale;
    dec->qscale_above = dec->qscale + 1;
    dec->qscale_left = dec->qscale + dec->mb_width + 1;
    */
    dec->qscale_current = dec->qscale;
    dec->qscale_left = dec->qscale + 1;
    dec->qscale_above = dec->qscale + 2;


    temp_size = (sizeof(int16_t) * (dec->mb_width+2)) << 5;
    dec->ac_pred_base = (int16_t *)xvid_malloc(temp_size,CACHE_LINE);
    if(dec->ac_pred_base)
        memset(dec->ac_pred_base,0,temp_size);
    else
    {
        goto memory_error;
    }
    dec->ac_pred_above_y = dec->ac_pred_base + 16;
    dec->ac_pred_left_y = dec->ac_pred_above_y + (dec->mb_width<<4);
    dec->ac_pred_above_u = dec->ac_pred_left_y + 24;
    dec->ac_pred_left_u = dec->ac_pred_above_u +(dec->mb_width<<3);
    dec->ac_pred_above_v = dec->ac_pred_left_u + 16;
    dec->ac_pred_left_v = dec->ac_pred_above_v +(dec->mb_width<<3);

    temp_size = (sizeof(VECTOR) * (dec->mb_width + 2)) << 2;
    dec->mv_buffer_base = (VECTOR *)xvid_malloc(temp_size,CACHE_LINE);
    if(dec->mv_buffer_base)
        memset(dec->mv_buffer_base,0,temp_size);
    else
    {
        goto memory_error;
    }


    temp_size = sizeof(int16_t) * 384;
    dec->block_size = temp_size;
    dec->block = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->block)
        memset(dec->block,0,temp_size);
    else
    {
        goto memory_error;
    }

    dec->data = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->data)
        memset(dec->data,0,temp_size);
    else
    {
        goto memory_error;
    }
    return 0;

memory_error:
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);

    xvid_free(dec);
    return XVID_ERR_MEMORY;
}


int
decoder_create(xvid_dec_create_t * create)
{
    DECODER *dec;

    //cyg_mutex_init(&vld_access_mutex);

    if (XVID_VERSION_MAJOR(create->version) != 1) /* v1.x.x */
        return XVID_ERR_VERSION;
//  frame_number = 0;//test by longyl
    dec = xvid_malloc(sizeof(DECODER), CACHE_LINE);
    if (dec == NULL)
    {
        return XVID_ERR_MEMORY;
    }

    memset(dec, 0, sizeof(DECODER));
    create->handle = dec;

    dec->width = create->width;
    dec->height = create->height;

    image_null(&dec->cur);
    image_null(&dec->refn[0]);
// image_null(&dec->tmp);

    dec->ac_pred_base = NULL;
    dec->mv_buffer = NULL;
    dec->qscale = NULL;
    dec->block = NULL;
    dec->data = NULL;


    /* For B-frame support (used to save reference frame's time */
    dec->frames = 0;
    dec->time = dec->time_base = 0;
    dec->low_delay = 0;
    dec->time_inc_resolution = 1; /* until VOL header says otherwise */
    dec->ver_id = 1;

    dec->bs_version = (short)0xffff; /* Initialize to very high value -> assume bugfree stream */

    dec->fixed_dimensions = (dec->width > 0 && dec->height > 0);

    if (dec->fixed_dimensions)
        return decoder_resize(dec);
    else
        return 0;
}


int
decoder_destroy(DECODER * dec)
{
    xvid_free(dec->ac_pred_base);
    xvid_free(dec->mv_buffer_base);
    xvid_free(dec->qscale);
    xvid_free(dec->block);
    xvid_free(dec->data);

    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    xvid_free(dec);

    return 0;
}
#endif

static const int32_t dquant_table[4] =
{
    -1, -2, 1, 2
};

//static int dongwl_cnt;

//static int inter_base, inter_bias, inter_cnt, inter_total;
#if 0
// only for intra
void mpeg4_hw_iq_idct(int frame_type, uint32_t iDcScaler, uint32_t iQuant,short * block,int i, short * data)
{

    int16_t * pInblock =block;

    unsigned int * temp_p;
    int ind;
    unsigned int tempdata;
    temp_p = (unsigned int *)data;

    //dongwl set IQ_IDCT reg Ctrl_iq_idct
    tempdata =(LILY_RD_REG(MP4_CTRL_IQ_IDCT) & 0xff0000ff) |((iDcScaler&0xff)<<0x8) |((iQuant&0xff)<<0x10) |0x04;
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,tempdata);

    //set interrupt mask intm_vld
    LILY_WR_REG(MP4_INTM_VLD,0X0);
    //input data
    for(ind=0; ind<64; ind+=2)
        LILY_WR_REG(MP4_VLD_BUF+ind*2, (pInblock[ind+0]&0xffff)| (pInblock[ind+1]<<0x10));

    //start Ctrl_iq_idct
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x1);
    //waitting... Stat_iq_idct
    while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);

    for (ind = 0; ind<32; ind++)
    {
        temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);
    }

    //stop
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);

}

// only for intra
void mpeg4_hw_vld(int frame_type,Bitstream * bs,int hw_direction,int dc_dif ,short * block,uint32_t i, short *data)
{

    unsigned int tempdata;
    unsigned int vld_length;
    unsigned int bufa;
    int index;

    int * block1;

    unsigned int dwl_bias, dwl_jmp;
    unsigned int dwl_base;

    int vld_cnt=0;


    block1 = block;

    //this is must be!! mohongfee 2007.08.28
    ifc_ch_disable();

    dwl_bias=((bs->pos +7) & 0xfffffff8) -bs->pos ;
    dwl_base=(unsigned int)(bs->tail-((bs->pos +7)>>3));

    dwl_jmp= (dwl_base-0x80000000)& 0x03;
    dwl_base -=dwl_jmp;
    dwl_bias +=(dwl_jmp<<3);


    ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, (dwl_base-0x80000000));//DATA bit stream buffer
    LILY_WR_REG(MP4_VLD_BIAS,dwl_bias);//BIAS Frame start bits bias

    //ifc_ch_enable();
    tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
    tempdata|=IFC_CH_ENABLE;
    LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);


    //dongwl set vld reg Ctrl_vld
    tempdata =(LILY_RD_REG(MP4_CTRL_VLD) & 0xffffffcf) | 0x00000084 | (hw_direction<<4) ;
    LILY_WR_REG(MP4_CTRL_VLD, tempdata);

    //disable idct after vld Ctrl_vld_iq_idct
    LILY_WR_REG(MP4_CTRL_VLD_IQ_IDCT, 0x0);

    //mp4_enb_vld();
    LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD)|0x01);

    //wait... Stat_vld
    while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0)
    {
//          diag_printf("########     hw_vld_run %d##########!\n",dongwl_cnt);
        vld_cnt++;

        if(vld_cnt>100)
        {
            diag_printf("########     hw_vld_run enter rolling still%d##########!\n");
            while(1);
        }
    }

    for (index=0; index<32; index++)
    {
        block1[index] = LILY_RD_REG(MP4_VLD_BUF+4*index);

    }

    block1[0] =(block1[0] &0xffff0000)|((int)dc_dif&0xffff);

    //get vld comsumed length LENGTH
    vld_length = LILY_RD_REG(MP4_VLD_LENGTH);

    //disable vld
    LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffd)|0x2);

    bs->tail =dwl_base+((dwl_bias+vld_length+7)>>3);
    bs->pos = ((dwl_bias+vld_length+7) & 0xfffffff8) - vld_length -dwl_bias;

    bufa=*(bs->tail -1);
    bs->buf=bufa;

}
#endif

static   uint8_t mpeg4_av_clip_uint8_c(int a)
{
    if (a&(~0xFF)) return (-a)>>31;
    else           return a;
}
const uint16_t *
get_inter_matrix(const uint16_t * mpeg_quant_matrices)
{
    return(mpeg_quant_matrices + 4*64);
}

#ifndef VOC_DO


static void put_mpeg4_pixels_clamped4_c(uint8_t *pixels,const short *block,
                                        int line_size)
{
    int i;

    /* read the pixels */
    for(i=0; i<4; i++)
    {
        pixels[0] = mpeg4_av_clip_uint8_c(block[0]);
        pixels[1] = mpeg4_av_clip_uint8_c(block[1]);
        pixels[2] = mpeg4_av_clip_uint8_c(block[2]);
        pixels[3] = mpeg4_av_clip_uint8_c(block[3]);

        pixels += line_size;
        block += 8;
    }
}

/* decode an intra macroblock */
static void
decoder_mbintra(DECODER * dec,
                const uint32_t x_pos,
                const uint32_t y_pos,
                const int16_t acpred_flag,
                const uint32_t cbp,
                Bitstream * bs,
                const uint32_t quant,
                const uint32_t intra_dc_threshold,
                const unsigned int bound)
{
    int16_t *block = dec->block;
    int16_t *data = dec->data;
    uint16_t next_block;
    uint16_t i;
    int16_t iQuant = quant;
    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

    int dc_dif;
    //add by longyl
#ifdef longyl
    int16_t *pCoeffBuffRow;
    int16_t *pCoeffBuffCol;
    int16_t *pQpBuffAbove;
    int16_t *pQpBuffLeft;
    int16_t *dct_codes;
    int16_t top,left,left_top;
    int16_t acpred_dir;
    int16_t predQp;
    int16_t dc_pred,dc;
    uint16_t acpred_bound_flag;
    uint32_t iDcScaler_mult;
    int16_t dc_tmp;
#endif
    int16_t fLeft, fTop, fDiag,mbpos;//added by liyongjian

    if(mpeg4_lowres ==1)
        next_block = dec->edged_width << 2;
    else
        next_block = dec->edged_width << 3;

    if(mpeg4_lowres==1)
    {
        pY_Cur = dec->y_row_start + (x_pos<<3);
        pU_Cur = dec->u_row_start + (x_pos<<2);
        pV_Cur = dec->v_row_start + (x_pos<<2);

    }
    else
    {
        pY_Cur = dec->y_row_start + (x_pos<<4);
        pU_Cur = dec->u_row_start + (x_pos<<3);
        pV_Cur = dec->v_row_start + (x_pos<<3);
    }
    pQpBuffLeft = dec->qscale_left;
    pQpBuffAbove = dec->qscale_above + x_pos;

    memset(block, 0, dec->block_size); /* clear */

    for (i = 0; i < 6; i++)
    {
        uint8_t iDcScaler = MPEG_DC_SCALAR[i < 4][iQuant];
#ifndef longyl
        int16_t predictors[8];
#endif
        int start_coeff;

        predQp = iQuant;
        acpred_bound_flag = acpred_flag;

        if(i < 4)
        {
            pCoeffBuffRow = dec->ac_pred_above_y + (x_pos<<4) + ((i&1)<<3);
            pCoeffBuffCol = dec->ac_pred_left_y + ((i>>1)<<3);
        }
        else if(i == 4)
        {
            pCoeffBuffRow = dec->ac_pred_above_u + (x_pos<<3);
            pCoeffBuffCol = dec->ac_pred_left_u;
        }
        else
        {
            pCoeffBuffRow = dec->ac_pred_above_v +(x_pos<<3);
            pCoeffBuffCol = dec->ac_pred_left_v;
        }
#if 0
        top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
        left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
        left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
#else

        fLeft = fTop = fDiag = 0;
        left = top = left_top = 0;
        mbpos = y_pos * dec->mb_width + x_pos;
        if(mbpos >= bound + 1)
            fLeft = 1;
        if(mbpos >= bound + dec->mb_width)
            fTop = 1;
        if(mbpos >= bound + dec->mb_width + 1)
            fDiag = 1;


        switch (i)
        {

            case 0:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;

                break;

            case 1:
                left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                if (fTop)
                {
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                }
                else
                {
                    left_top = top = 0x400;
                }
                break;

            case 2:
                if (fLeft)
                {
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                }
                else
                {
                    left_top = left = 0x400;
                }

                top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));

                break;

            case 3:
                left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));

                break;

            case 4:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;
                break;

            case 5:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;
                break;

        }
#endif

        if(ABS((left-left_top)) < ABS((top-left_top)))//abs -->ABS error.shenh.08.11.18
        {
            acpred_dir = 1;//vertical
            dc_pred = top;
            acpred_bound_flag -= (((*pCoeffBuffRow)>>15)&acpred_bound_flag);
            if(i<2 || i>3)
                predQp = *pQpBuffAbove;
        }
        else
        {
            acpred_dir = 2;//horizontal
            dc_pred = left;
            acpred_bound_flag -= (((*pCoeffBuffCol)>>15)&acpred_bound_flag);
            if((i!=1)&&(i!=3))
                predQp = *pQpBuffLeft;
        }
        acpred_dir = acpred_dir - (((acpred_flag-1)>>15)&acpred_dir);

        if(quant < intra_dc_threshold)
        {
            int dc_size;


            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            dc_size = i < 4 ? get_dc_size_lum(bs) : get_dc_size_chrom(bs);
            dc_dif = dc_size ? get_dc_dif(bs, dc_size) : 0;
            bs->pos = bs->pos - (((8 - dc_size)>>31)&1);
            block[i << 6] = dc_dif;
            start_coeff = 1;
        }
        else
        {
            start_coeff = 0;
        }

        if (cbp & (32 >> i)) /* coded */
        {
            int hw_direction = 0;


            if(acpred_dir==1)
                hw_direction = 2;
            else if(acpred_dir ==2)
                hw_direction = 1;

///         diag_printf("########     enter mpeg4 vld part  %d##########!\n", dongwl_cnt);
            //dongwl_cnt++;

//          cyg_mutex_lock(&vld_access_mutex);

            //cyg_scheduler_lock();

            //cyg_interrupt_disable();

///         diag_printf("######## buf1 is 0x%x, pos is %d!\n", bs->buf, bs->pos);


            get_intra_block(bs, &block[i << 6], acpred_dir, start_coeff);
            //mpeg4_hw_vld(0,  bs, hw_direction,  dc_dif, &block[i << 6], i, &data[i << 6]);



            //cyg_interrupt_enable();

            //cyg_scheduler_unlock();

///         diag_printf("######## buf2 is 0x%x, pos is %d!\n", bs->buf, bs->pos);
///         diag_printf("\n\n");

///         if(dongwl_cnt>12)
///             while(1);


//          cyg_mutex_unlock(&vld_access_mutex);

        }

        dct_codes = &block[i << 6];
        dc = dct_codes[0];
        iDcScaler_mult = MPEG_DC_SCALAR_MULT[i<4][iQuant];
        dc += ((dc_pred*iDcScaler_mult + 0x10000)>>17) + ((dc_pred >> 15)&0xffff);
        dct_codes[0] = dc;
        dc *= iDcScaler;
        dc = dc + (((dc+2048) >> 15)&(-2048 - dc));
        dc = 2047 + (((dc-2047) >> 15)&(dc - 2047));

        if(acpred_dir == 1)
        {
            //ver
            int16_t j;
            int level;
            for(j = 1; j < 8; j++)
            {
                if (pCoeffBuffRow[j] ==0)
                    level = dct_codes[j] ;
                else
                    level = dct_codes[j]+ (((pCoeffBuffRow[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15);

//              int level = dct_codes[j] + (((pCoeffBuffRow[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15); // + ((pCoeffBuffRow[j]>>15)&0xffff);

                dct_codes[j] = level + (((acpred_bound_flag-1)>>15)&(dct_codes[j]-level));
                pCoeffBuffRow[j] = dct_codes[j];
                pCoeffBuffCol[j] = dct_codes[j<<3];
            }
        }
        else if(acpred_dir == 2)
        {
            //hor
            int16_t j;
            int level;
            for(j = 1; j < 8; j++)
            {
                if (pCoeffBuffCol[j] ==0)
                    level = dct_codes[j<<3] ;
                else
                    level = dct_codes[j<<3] + (((pCoeffBuffCol[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15);
//              int level = dct_codes[j<<3] + (((pCoeffBuffCol[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15); // + ((pCoeffBuffCol[j]>>15)&0xffff);

                dct_codes[j<<3] = level + (((acpred_bound_flag-1)>>15)&(dct_codes[j<<3]-level));
                pCoeffBuffCol[j] = dct_codes[j<<3];
                pCoeffBuffRow[j] = dct_codes[j];
            }
        }
        else
        {
            int16_t j;

            for(j = 1; j < 8; j++)
            {
                pCoeffBuffRow[j] = dct_codes[j];
                pCoeffBuffCol[j] = dct_codes[j<<3];
            }
        }

        switch (i)
        {
            case 1:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = *pCoeffBuffRow;
                *pCoeffBuffRow = dc;

                break;
            case 3:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                * pCoeffBuffCol = dc;
                dc_tmp = *pCoeffBuffRow;
                *pCoeffBuffRow = * (pCoeffBuffCol - 8);
                * (pCoeffBuffCol - 8) = dc_tmp;

                break;
            default:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = dc;
                break;
        }
        if(dec->quant_type==0)
        {
            dequant_h263_intra(&data[i<<6], &block[i<<6], iQuant, iDcScaler,dec->mpeg_quant_matrices);
        }
        else
        {
            dequant_mpeg_intra(&data[i<<6], &block[i<<6], iQuant,iDcScaler,dec->mpeg_quant_matrices);
        }
        if(mpeg4_lowres==1)
        {
            mpeg4_idct16(&data[i<<6]);
        }
        else
        {
            //mpeg4_hw_iq_idct(0, iDcScaler, iQuant,  &block[i << 6],  i, &data[i << 6]);
#ifdef SHEEN_VC_DEBUG
            idct_int32((short * const)&data[i << 6]);
#endif
        }
    }
    *pQpBuffAbove = *pQpBuffLeft = iQuant;
    if(mpeg4_lowres==1)
    {
        put_mpeg4_pixels_clamped4_c(pY_Cur, &data[0], dec->stride[0]);
        put_mpeg4_pixels_clamped4_c(pY_Cur + 4, &data[64], dec->stride[1]);
        put_mpeg4_pixels_clamped4_c(pY_Cur + next_block, &data[128], dec->stride[2]);
        put_mpeg4_pixels_clamped4_c(pY_Cur + 4 + next_block, &data[192], dec->stride[3]);
        put_mpeg4_pixels_clamped4_c(pU_Cur, &data[256], dec->stride[4]);
        put_mpeg4_pixels_clamped4_c(pV_Cur, &data[320], dec->stride[5]);

    }
    else
    {

#ifdef SHEEN_VC_DEBUG
        transfer_16to8copy(pY_Cur, &data[0], dec->stride[0]);
        transfer_16to8copy(pY_Cur + 8, &data[64], dec->stride[1]);
        transfer_16to8copy(pY_Cur + next_block, &data[128], dec->stride[2]);
        transfer_16to8copy(pY_Cur + 8 + next_block, &data[192], dec->stride[3]);
        transfer_16to8copy(pU_Cur, &data[256], dec->stride[4]);
        transfer_16to8copy(pV_Cur, &data[320], dec->stride[5]);

#else //for mips idct
        mpeg4_simple_idct_put(pY_Cur, (int)dec->stride[0], data);
        mpeg4_simple_idct_put(pY_Cur+8, (int)dec->stride[1], data+64);
        mpeg4_simple_idct_put(pY_Cur + next_block, (int)dec->stride[2], data+128);
        mpeg4_simple_idct_put(pY_Cur + 8 + next_block, (int)dec->stride[3], data+192);
        mpeg4_simple_idct_put(pU_Cur, (int)dec->stride[4], data+256);
        mpeg4_simple_idct_put(pV_Cur, (int)dec->stride[5], data+320);
#endif
    }
}


static void mpeg4_add_pixels_clamped4_c(const short *block, uint8_t *pixels,
                                        int line_size)
{
    int i;

    /* read the pixels */
    for(i=0; i<4; i++)
    {
        pixels[0] = mpeg4_av_clip_uint8_c(pixels[0] + block[0]);
        pixels[1] = mpeg4_av_clip_uint8_c(pixels[1] + block[1]);
        pixels[2] = mpeg4_av_clip_uint8_c(pixels[2] + block[2]);
        pixels[3] = mpeg4_av_clip_uint8_c(pixels[3] + block[3]);
        pixels += line_size;
        block += 8;
    }
}


static void
decoder_mb_decode(DECODER * dec,
                  uint32_t cbp,
                  Bitstream * bs,
                  uint8_t * pY_Cur,
                  uint8_t * pU_Cur,
                  uint8_t * pV_Cur)
{
    int16_t *data = dec->data;

    int i;
    const uint32_t iQuant = *dec->qscale_current;

    uint8_t *dst[6];
    uint16_t    next_block;
    if(mpeg4_lowres ==1)
    {
        next_block= dec->edged_width << 2;
        dst[0] = pY_Cur;
        dst[1] = pY_Cur + 4;
        dst[2] = pY_Cur + next_block;
        dst[3] = pY_Cur + next_block+4;
        dst[4] = pU_Cur;
        dst[5] = pV_Cur;

    }
    else
    {
        dst[0] = pY_Cur;
        dst[1] = pY_Cur + 64;
        dst[2] = pY_Cur + 128;
        dst[3] = pY_Cur + 192;
        dst[4] = pU_Cur;
        dst[5] = pV_Cur;
    }
    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (32 >> i))
        {
            /* Clear the block */
            memset(&data[0], 0, 128);

            /* Decode coeffs and dequantize on the fly */
            if(dec->quant_type == 0)
            {
                get_inter_block_h263(bs, &data[0], 0, iQuant);
            }
            else
            {
                get_inter_block_mpeg(bs, &data[0], 0, iQuant, get_inter_matrix(dec->mpeg_quant_matrices));
            }
            if(mpeg4_lowres==1)
            {
                mpeg4_idct16(&data[0]);
                mpeg4_add_pixels_clamped4_c(data,dst[i],dec->stride[i]);
            }
            else
            {
#ifdef SHEEN_VC_DEBUG
                /* iDCT */
                idct_int32((short * const)&data[0]);

                /* Add this residual to the predicted block */
                transfer_16to8add(dst[i], &data[0], dec->stride[i]);
#else
                mpeg4_simple_idct_add(dst[i], 8, data);
#endif
            }
        }
    }
}
#endif

#if 0
static void
decoder_mb_decode_hardBad(DECODER * dec,
                          uint32_t cbp,
                          Bitstream * bs,
                          uint8_t * pY_Cur,
                          uint8_t * pU_Cur,
                          uint8_t * pV_Cur)
{
    int16_t *data = dec->data;

    unsigned int inter_jmp, fpga_bias;
    unsigned int *fpga_base;

    int i, bFlag;
    const uint32_t iQuant = *dec->qscale_current;
    uint8_t *dst[6];

    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 64;
    dst[2] = pY_Cur + 128;
    dst[3] = pY_Cur + 192;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;


    inter_cnt=inter_total=0;
    inter_base = (unsigned int)(bs->tail-((bs->pos +7)>>3));
    inter_bias = ((bs->pos +7) & 0xfffffff8) -bs->pos ;

    inter_jmp =(inter_base-0x80000000)& 0x03;
    fpga_base =( inter_base - inter_jmp);
    fpga_bias = inter_bias + (inter_jmp<<3);


    bFlag=8;

    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (32 >> i))
        {
            unsigned int tempdata;
            unsigned int vld_length;

            unsigned int * temp_p;

            temp_p = &data[0];
            int ind;

            //this is must be!! mohongfee 2007.08.28
            ifc_ch_disable();

            ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, ( fpga_base -0x80000000));
            LILY_WR_REG(MP4_VLD_BIAS, fpga_bias);
            //ifc_ch_enable();
            tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
            tempdata|=IFC_CH_ENABLE;
            LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);

            //dongwl set vld reg
            tempdata=(LILY_RD_REG(MP4_CTRL_VLD) & 0xffffffcb) | 0x0080;
            LILY_WR_REG(MP4_CTRL_VLD, tempdata);

            //dongwl set IQ_IDCT reg
            tempdata=(LILY_RD_REG(MP4_CTRL_IQ_IDCT) & 0xff00fffb) | ((iQuant&0xff)<<0x10) ;
            LILY_WR_REG(MP4_CTRL_IQ_IDCT, tempdata);


            LILY_WR_REG(MP4_CTRL_VLD_IQ_IDCT, 0x00000001);

            LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD) |0x00000001);

            if(bFlag != 8)
            {
                transfer_16to8add(dst[bFlag], &data[0], dec->stride[bFlag]);
            }
            bFlag=i;

            //waitting... vld
            while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0);

            //get vld comsumed length
            vld_length = LILY_RD_REG(MP4_VLD_LENGTH);

            inter_total +=vld_length;
            fpga_base += (fpga_bias + vld_length)>>5;
            fpga_bias = (fpga_bias + vld_length)%32;

            //waitting... iq idct
            while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);

            for (ind = 0; ind<32; ind++)
            {
                temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);

            }

            //stop vld
            LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD) |0x02);

            //stop iq idct
            LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);

        }
    }


    if(bFlag != 8)
    {
        transfer_16to8add(dst[bFlag], &data[0], dec->stride[bFlag]);
    }

    bs->tail = inter_base+((inter_bias+inter_total+7)>>3);
    bs->pos = ((inter_bias+inter_total+7) & 0xfffffff8) - inter_total -inter_bias;
    bs->buf=*(bs->tail -1);

}


static void
decoder_mb_decode_hardOK(DECODER * dec,
                         const uint32_t cbp,
                         Bitstream * bs,
                         uint8_t * pY_Cur,
                         uint8_t * pU_Cur,
                         uint8_t * pV_Cur
                        )

{
    int16_t *data = dec->data;

    unsigned int inter_jmp, fpga_bias;
    unsigned int *fpga_base;

    int i, bFlag;
    const uint32_t iQuant = *dec->qscale_current;
    uint8_t *dst[6];
    unsigned int * temp_p;

    temp_p = &data[0];

    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 64;
    dst[2] = pY_Cur + 128;
    dst[3] = pY_Cur + 192;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;


    inter_cnt=inter_total=0;
    inter_base = (unsigned int)(bs->tail-((bs->pos +7)>>3));
    inter_bias = ((bs->pos +7) & 0xfffffff8) -bs->pos ;

    inter_jmp =(inter_base-0x80000000)& 0x03;
    fpga_base =( inter_base - inter_jmp);
    fpga_bias = inter_bias + (inter_jmp<<3);


    bFlag=8;


    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (1 << (5 - i)))
        {

            {
                unsigned int tempdata;
                unsigned int vld_length;


                int ind;

                memset(&data[0], 0, 128);

                //put_string("******:", 6);
                //put_enter();

                //this is must be!! mohongfee 2007.08.28
                ifc_ch_disable();
                ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, (fpga_base-0x80000000));
                LILY_WR_REG(MP4_VLD_BIAS,fpga_bias);

                //ifc_ch_enable();
                tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
                tempdata|=IFC_CH_ENABLE;
                LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);
                //vld inter block
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffb));
                //vld scan type, here zigzag type
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xffffffcf)|0x0);
                //vld endian set
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xffffff7f)|0x80);


                //mp4_enb_vld();
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffe)|0x1);

                //wait...
                while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0);
//////////////////////////////-------------------------------------------------
                for (ind=0; ind<32; ind++)
                {
                    temp_p[ind] = LILY_RD_REG(MP4_VLD_BUF+4*ind);

                }


                //get vld comsumed length
                vld_length = LILY_RD_REG(MP4_VLD_LENGTH);
                //disable vld
                //mp4_dis_vld();
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffd)|0x2);

                //after hardware vld; we should cope with the bs data structure.
                //bs->tail = (bs->tail) + ((bs->pos+vld_length)>>5);
                //bs->pos = (bs->pos+vld_length)%32;
                //bufa = *(bs->tail);
                //bufb = *(bs->tail+1);
                inter_total +=vld_length;
                fpga_base += (fpga_bias + vld_length)>>5;
                fpga_bias = (fpga_bias + vld_length)%32;


                //bs->bufa = bufa;
                //bs->bufb = bufb;
            }

            bs->tail = inter_base+((inter_bias+inter_total+7)>>3);
            bs->pos = ((inter_bias+inter_total+7) & 0xfffffff8) - inter_total -inter_bias;
            bs->buf=*(bs->tail -1);

            // mpeg4_hw_iq_idct(1, 0,  iQuant,  &temp_p[0],  i,  data);
            {
                int16_t * pInblock = &data[0];
                int16_t * pOutblock = &data[0];
                unsigned int * temp_p;
                unsigned int utemp = 0;
                temp_p = (unsigned int *)pOutblock;
                int ind;

                //set inter
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffb));
                //set iQuant
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xff00ffff)|((iQuant&0xff)<<0x10));
                //set interrupt mask
                LILY_WR_REG(MP4_INTM_VLD,0X0);
                //input data
                for(ind=0; ind<64; ind+=2)
                    LILY_WR_REG(MP4_VLD_BUF+ind*2, (pInblock[ind+0]&0xffff)|(pInblock[ind+1]<<0x10));
                //start
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x1);
                //waitting...
                while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);
                //output data



                for (ind = 0; ind<32; ind++)
                {
                    temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);

                }
                //stop
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);
            }
            /* Add this residual to the predicted block */
            transfer_16to8add(dst[i], &data[0], dec->stride[i]);
        }
    }


}
#endif

#ifndef SHEEN_VC_DEBUG
extern void (*fillAudioData)();//read audio data
extern uint16 MBScale4AudRed;
//int32 tt1,tt2, totc,totv;
#endif

void
decoder_iframe(DECODER * dec,
               Bitstream * bs,
               int quant,
               int intra_dc_threshold)
{
    uint16_t bound;
    int16_t x, y;//i add by zhangyongheng
    const int16_t mb_width = dec->mb_width;
    const int16_t mb_height = dec->mb_height;
#ifndef SHEEN_VC_DEBUG
    uint16 fillAudMbScale=MBScale4AudRed;//big than qcif
#endif

#ifndef VOC_DO
    //int temp;
    uint16_t temp_code;
    //int16_t *pQpBuffAbove;
    //int16_t *pQpBuffLeft;
    //int16 *test5;
    //int8 sendVocReq;
#else
    int i,temp1;
    UINT32 cs_status;
#endif

//totc=totv=0;//test

    if(dec->mb_width==0||dec->mb_height==0)
        return;
    bound = 0;
////////////add by longyl
#ifdef longyl
    for(x = -1; x < mb_width; x ++)
    {
        *(dec->ac_pred_above_y + (x << 4)) = -1;
        *(dec->ac_pred_above_y + (x << 4) +8) = -1;
        *(dec->ac_pred_above_u + (x << 3)) = -1;
        *(dec->ac_pred_above_v + (x << 3)) = -1;
    }
    *(dec->ac_pred_left_y) = -1;
    *(dec->ac_pred_left_y + 8) = -1;
    *(dec->ac_pred_left_u) = -1;
    *(dec->ac_pred_left_v) = -1;
    dec->y_row_start = dec->cur.y;
    dec->u_row_start = dec->cur.u;
    dec->v_row_start = dec->cur.v;
#endif


#if 0//print table for voc
    {
        int i,j;
        FILE *pH;
        uint8_t *p;

        pH=fopen("E:\\projects\\gallite_hx_simu\\platform_simu\\voc\\simus\\mpeg4_voc\\vpp_mpeg4_dec_const_y.tab","wb");

        fprintf(pH, "/* MPEG_DC_SCALAR */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<16; j++)
            {
                fprintf(pH, "0x%04hx",MPEG_DC_SCALAR[i][j*2+1]);
                fprintf(pH, "%04hx,",MPEG_DC_SCALAR[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* dc_lum_tab_voc */ \n");
        for(i=0; i<8; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",dc_lum_tab_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",dc_lum_tab_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* MPEG_DC_SCALAR_MULT */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<16; j++)
            {
                fprintf(pH, "0x%04hx",MPEG_DC_SCALAR_MULT[i][j*2+1]);
                fprintf(pH, "%04hx,",MPEG_DC_SCALAR_MULT[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* iQuant_MULT */ \n");
        for(j=0; j<16; j++)
        {
            fprintf(pH, "0x%04hx",iQuant_MULT[j*2+1]);
            fprintf(pH, "%04hx,",iQuant_MULT[j*2]);
            fprintf(pH, "\n");
        }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* const_number */ \n");
        for(j=0; j<32; j++)
        {
            fprintf(pH, "0x%04hx",const_number[j*2+1]);
            fprintf(pH, "%04hx,",const_number[j*2]);
            fprintf(pH, "\n");
        }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* scan_tables */ \n");
        for(i=0; i<3; i++)
            for(j=0; j<32; j++)
            {
                fprintf(pH, "0x%04hx",scan_tables[i][j*2+1]);
                fprintf(pH, "%04hx,",scan_tables[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* max_level_voc */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<32; j++)
            {
                fprintf(pH, "0x%04hx",max_level_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",max_level_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* max_run_voc */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<32; j++)
            {
                fprintf(pH, "0x%04hx",max_run_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",max_run_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* mcbpc_inter_table_voc */ \n");
        for(i=0; i<257; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",mcbpc_inter_table_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",mcbpc_inter_table_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* cbpy_table_voc */ \n");
        for(i=0; i<64; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",cbpy_table_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",cbpy_table_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* TMNMVtab0voc */ \n");
        for(i=0; i<14; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",TMNMVtab0voc[i][j*2+1]);
                fprintf(pH, "%04hx,",TMNMVtab0voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* TMNMVtab1voc */ \n");
        for(i=0; i<96; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",TMNMVtab1voc[i][j*2+1]);
                fprintf(pH, "%04hx,",TMNMVtab1voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* TMNMVtab2voc */ \n");
        for(i=0; i<124; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",TMNMVtab2voc[i][j*2+1]);
                fprintf(pH, "%04hx,",TMNMVtab2voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* mcbpc_intra_table_voc */ \n");
        for(i=0; i<64; i++)
            for(j=0; j<1; j++)
            {
                fprintf(pH, "0x%04hx",mcbpc_intra_table_voc[i][j*2+1]);
                fprintf(pH, "%04hx,",mcbpc_intra_table_voc[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* roundtab_76 */ \n");
        for(j=0; j<8; j++)
        {
            fprintf(pH, "0x%04hx",roundtab_76[j*2+1]);
            fprintf(pH, "%04hx,",roundtab_76[j*2]);
            fprintf(pH, "\n");
        }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* max_level_h263 */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<32; j++)
            {
                fprintf(pH, "0x%04hx",max_level_h263[i][j*2+1]);
                fprintf(pH, "%04hx,",max_level_h263[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fprintf(pH, "/* max_run_h263 */ \n");
        for(i=0; i<2; i++)
            for(j=0; j<32; j++)
            {
                fprintf(pH, "0x%04hx",max_run_h263[i][j*2+1]);
                fprintf(pH, "%04hx,",max_run_h263[i][j*2]);
                fprintf(pH, "\n");
            }
        fprintf(pH, "\n\n");

        fflush(pH);
        fclose(pH);

    }
#endif
///////////////////////////////////////////
    for (y = 0; y < mb_height; y++)
    {
        for (x = 0; x < mb_width; x++)
        {

#ifndef VOC_DO
            uint16_t mcbpc;
            uint16_t cbpc;
            int16_t acpred_flag;
            uint16_t cbpy;
            uint16_t cbp;
#endif

#ifdef VOC_DO

            temp1 = 4- ((INT32)bs->tail & 0x3);
            if (temp1 != 4)
            {
                if(((bs->pos>>3)+1 +temp1) > 4)
                {
                    bs->buf = (bs->buf) >>((4-temp1)<<3);
                    bs->pos = bs->pos - ((4-temp1)<<3);
                    bs->tail = bs->tail - (4-temp1);

                }
                else
                {
                    while(temp1>0)
                    {
                        _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
                        temp1--;
                    }
                }
            }

#ifndef SHEEN_VC_DEBUG

            while(*(vocWorkState.pVOC_WORK) ==1/*audio*/ ||(*(vocWorkState.pVOC_WORK) ==2/*video*/ && *(vocWorkState.pVOC_VID) !=0))
            {
                //wait last voc vid done
                //diag_printf("1 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                //sxr_Sleep(1);
            }

            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=2;//video
            *(vocWorkState.pVOC_VID)=2;//more voc work to do

            hal_SysExitCriticalSection(cs_status);
#endif

            //debug
            //memset(hal_VocGetPointer(3244),0xcc,4368);
            //memset(hal_VocGetPointer(0x4000 +74+7456),0xcc,9654 +322 -7456);
// tt1=hal_TimGetUpTime();
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_MB_DEC_ID_ADDR)))= 0;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_x_pos_ADDR)))= ((INT32)(x))|(((INT32)(y))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_mb_width_ADDR)))= ((INT32)(dec->mb_width))|(((INT32)(dec->mb_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_edged_width_ADDR)))= ((INT32)(dec->edged_width))|(((INT32)(dec->edged_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_width_ADDR)))= ((INT32)(dec->width))|(((INT32)(dec->height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_bs_tail_ADDR)))= ((INT32)(0))|(((INT32)(quant))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_intra_dc_threshold_ADDR)))= intra_dc_threshold;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_bound_ADDR)))= bound;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_buf_EXTADDR)))= (INT32)bs->buf;
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_pos_ADDR)))= (INT16)bs->pos;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_tail_EXTADDR)))= ((INT32)bs->tail &VOC_CFG_DMA_EADDR_MASK)>>2;//for temp ext addr.
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_y_row_start_EXTADDR)))= (INT32)(dec->y_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_u_row_start_EXTADDR)))= (INT32)(dec->u_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_v_row_start_EXTADDR)))= (INT32)(dec->v_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_quant_bits_ADDR)))= ((INT32)(dec->quant_bits))|(((INT32)(dec->time_inc_bits))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_DMA_STATUS_ADDR)))=((INT32)mpeg4_lowres)<<16;//GLOBAL_MPEG4_BS_DMA_flag_ADDR is unused;THE HIGH ADDRESS IS GLOBAL_MPEG4_DEC_lowres_ADDR

#ifdef RAM_DCT3D
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)(DCT3D+4096) &VOC_CFG_DMA_EADDR_MASK)>>2;
#else
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D[0] &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)DCT3D[1] &VOC_CFG_DMA_EADDR_MASK)>>2;
#endif

            {
                INT32 *p1,*p2,size;

                *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_pQpBuffcurrent_ADDR)))= ((INT32)(*dec->qscale_current))|(((INT32)(*dec->qscale_left))<<16);
                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_pQpBuffAbove_ADDR));
                p2= (INT32*) (dec->qscale_above);
                size= (dec->mb_width)>>1;
                for(i=0; i<size; i++)
                    *p1++=*p2++;
                if(dec->mb_width&0x1)
                    *(INT16*)p1= *(INT16*)p2;

                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_ac_pred_base_ADDR));
                p2= (INT32*) (dec->ac_pred_above_y -8);
                size= (dec->mb_width+2) << 4;//(sizeof(int16_t) * (dec->mb_width+2)) << 5;
                for(i=0; i<size; i++)
                    *p1++=*p2++;
            }
// tt2=hal_TimGetUpTime();
//totc+=tt2-tt1;
            //CS_mpeg4_mbdec();
#ifndef SHEEN_VC_DEBUG

            vpp_AVDecSetMode(MMC_MPEG4_DECODE, 0, 0);
            hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

            //sendVocReq =0;
            while(*(vocWorkState.pVOC_VID) !=0)
            {
                //wait last voc vid done
                temp1=0;
                //start unblock display
                if(vocWorkState.vid_display)
                {
                    vocWorkState.vid_display();
                    vocWorkState.vid_display= NULL;
                }
                //audio decode first
                if(vocWorkState.aud_nframe_dec)
                    temp1=vocWorkState.aud_nframe_dec();
                //block display second
                /*if(temp1!=1 && vocWorkState.vid_display)
                {
                    vocWorkState.vid_display();
                    vocWorkState.vid_display= NULL;
                    //sxr_Sleep(1);
                }*/
                // read next frame
                if(temp1!=1 && vocWorkState.vid_frame_read)
                {
                    vocWorkState.vid_frame_read();
                    vocWorkState.vid_frame_read= NULL;
                }

                //if(*(vocWorkState.pVOC_AUD) >0 && !sendVocReq)
                //{
                //INT16 *p;
                //p= (INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_QUIT_VOC_ADDR -1));
                //*(p+1)= 1;
                //  sendVocReq=1;
                //}
                //diag_printf("2 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                sxr_Sleep(1);
            }

            vocWorkState.aud_nframe_dec=NULL;
            //aac will cover these ram below.!!!!!!!!!!!!!

#else
            /*
            VoC_set_event(CFG_WAKEUP_SOF0);

#ifdef BINARY_SIM
            VoC_execute(0);//offset 0x00 =Coolsand_AudioDecode_MAIN
#else
            Coolsand_AudioDecode_MAIN();
#endif
            */
            voc_codec_exe();

#endif

// tt1=hal_TimGetUpTime();
//totv+=tt1-tt2;

            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_tail_EXTADDR);//voc reuse MPEG4_bs_tail_EXTADDR to store lo16 pos
            bs->pos = (uint16)(bs->buf & 0xffff);//lo16 pos
            bs->tail += *(UINT32*) hal_VocGetPointer(MPEG4_bs_consume_ADDR);//reuse MPEG4_bs_consume_ADDR store size consume.
            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_buf_EXTADDR);//voc reuse MPEG4_bs_buf_EXTADDR to store buf

            {
                INT32 *p1,*p2;
                UINT32 tempVal;

                p2= (INT32*) hal_VocGetPointer(GLOBAL_MPEG4_ac_pred_base_ADDR);
                p1= (INT32*) (dec->ac_pred_above_y -8);
                tempVal= (dec->mb_width+2) << 4;//(sizeof(int16_t) * (dec->mb_width+2)) << 5;
                for(i=0; i<tempVal; i++)
                    *p1++=*p2++;

                quant= *((INT32*) hal_VocGetPointer(GLOBAL_MPEG4_bs_tail_ADDR));//hi16= quant
                quant>>=16;
                *(dec->qscale_left)= (*((INT32*) hal_VocGetPointer(GLOBAL_pQpBuffcurrent_ADDR)))>>16;//hi16 qscale_left
                p2= (INT32*) hal_VocGetPointer(GLOBAL_pQpBuffAbove_ADDR);
                p1= (INT32*) (dec->qscale_above);
                tempVal= (dec->mb_width)>>1;
                for(i=0; i<tempVal; i++)
                    *p1++=*p2++;
                if(dec->mb_width&0x1)
                    *(INT16*)p1= *(INT16*)p2;

                intra_dc_threshold= *((INT32*) hal_VocGetPointer(GLOBAL_intra_dc_threshold_ADDR));
                bound= *((INT32*) hal_VocGetPointer(GLOBAL_bound_ADDR));

                tempVal= *((UINT32*) hal_VocGetPointer(MPEG4_x_pos_ADDR));
                x= tempVal &0xFFFF;
                y= tempVal>>16;

                if(mpeg4_lowres==1)
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<3)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<1)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<1)*y;

                }
                else
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<4)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<2)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<2)*y;
                }

            }

#ifndef SHEEN_VC_DEBUG
            //free voc for audio
            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=0;//video
            if( y <(mb_height -1) || x< (mb_width -1))
                *(vocWorkState.pVOC_VID)=1;//more voc work to do
            //if(vocWorkState.vid_voc_isr)
            //  vocWorkState.vid_voc_isr();

            hal_SysExitCriticalSection(cs_status);
#endif

//tt2=hal_TimGetUpTime();
//totc+=tt2-tt1;

#else //VOC_DO


            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
#if 0
            _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            if (temp_code == 1)
                bs->pos -= 9;
#else
            _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            while(temp_code == 1)
            {
                bs->pos -= 9;
                if(bs->pos < 16)//sheen
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            }
#endif
            if (check_resync_marker(bs, 0))
            {
                bound = read_video_packet_header(bs, dec, 0,
                                                 &quant, NULL, NULL, &intra_dc_threshold);
                // x = bound % mb_width; avoid Avatar.mp4 crash
                // y = bound / mb_width;
            }
            {
                uint32_t index;

                if(bs->pos < 16)//sheen
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _SHOWBITS(bs->buf,bs->pos,9,index);
                index >>= 3;
                bs->pos -= mcbpc_intra_table[index].len;
                mcbpc = mcbpc_intra_table[index].code;
            }
            dec->mode = mcbpc & 7;
            cbpc = (mcbpc >> 4);

            if(bs->pos < 16)//sheen
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,1,acpred_flag);

            {
                uint32_t index;
                _SHOWBITS(bs->buf,bs->pos,6,index);
                bs->pos -= cbpy_table[index].len;
                cbpy = cbpy_table[index].code;
            }
            cbp = (cbpy << 2) | cbpc;

            if (dec->mode == MODE_INTRA_Q)
            {
                uint32_t index;
                _GETBITS(bs->buf,bs->pos,2,index);
                quant += dquant_table[index];
//      quant = 31 + (((quant-31)>>31)&(quant-31));
//      quant = 1 + (((1-quant)>>31)&(1-quant));
                if (quant > 31)
                {
                    quant = 31;
                }
                else if (quant < 1)
                {
                    quant = 1;
                }
            }
            *dec->qscale_current = quant;
            decoder_mbintra(dec, x, y, acpred_flag, cbp, bs, quant,
                            intra_dc_threshold, bound);

#endif //VOC_DO

        }
        /////add by longyl
#ifdef longyl
        *(dec->ac_pred_above_y + (dec->mb_width<<4) - 8) = *(dec->ac_pred_left_y +8);
        *(dec->ac_pred_above_u + (dec->mb_width<<3) - 8) = *(dec->ac_pred_left_u);
        *(dec->ac_pred_above_v + (dec->mb_width<<3) - 8) = *(dec->ac_pred_left_v);
        *(dec->ac_pred_left_y) = -1;
        *(dec->ac_pred_left_y + 8) = -1;
        *(dec->ac_pred_left_u) = -1;
        *(dec->ac_pred_left_v) = -1;

        if(mpeg4_lowres==1)
        {
            dec->y_row_start += dec->edged_width << 3;
            dec->u_row_start += dec->edged_width << 1;
            dec->v_row_start += dec->edged_width << 1;
        }
        else
        {
            dec->y_row_start += dec->edged_width << 4;
            dec->u_row_start += dec->edged_width << 2;
            dec->v_row_start += dec->edged_width << 2;
        }
#endif
        //////////////////////////////////////////////
#ifndef SHEEN_VC_DEBUG
#ifndef MEDIA_VOCVID_SUPPORT
        if(fillAudioData && (y*mb_width)>=fillAudMbScale)//read audio when image big than qcif
        {
            fillAudioData();
            fillAudMbScale+=MBScale4AudRed;
        }
#endif
#endif


        /*    if(dec->out_frm)
              output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,0,y,mb_width);*/
    }

    for (y = 0; y < mb_height; y++)
    {
        for (x = 0; x < mb_width; x++)
        {
            //MACROBLOCK *last_mb;
            int16_t *pLast_mbs_mode;
            VECTOR4 *pLast_mbs_mv;
            /*
             last_mb = &dec->last_mbs[y * dec->mb_width + x];
             last_mb->mode = 0;
             last_mb->mvs[0].x = last_mb->mvs[0].y =
               last_mb->mvs[1].x = last_mb->mvs[1].y =
               last_mb->mvs[2].x = last_mb->mvs[2].y =
               last_mb->mvs[3].x = last_mb->mvs[3].y =0;*/
            pLast_mbs_mode = &dec->last_mbs_mode[y * dec->mb_width + x];
            pLast_mbs_mv = &dec->last_mbs_mv[y * dec->mb_width + x];
            *pLast_mbs_mode = 0;
            pLast_mbs_mv->mvs[0].x = pLast_mbs_mv->mvs[0].y =
                                         pLast_mbs_mv->mvs[1].x = pLast_mbs_mv->mvs[1].y =
                                                 pLast_mbs_mv->mvs[2].x = pLast_mbs_mv->mvs[2].y =
                                                         pLast_mbs_mv->mvs[3].x = pLast_mbs_mv->mvs[3].y =0;
        }
    }

//diag_printf("i totc %d totv %d", totc*1000/16384, totv*1000/16384);

}


/* for P_VOP set gmc_warp to NULL */
void
decoder_pframe(DECODER * dec,
               Bitstream * bs,
               int rounding,
               int quant,
               int fcode,
               int intra_dc_threshold,
               const WARPPOINTS *const gmc_warp)
{

    uint16_t bound;
    int cp_mb, st_mb;
    int16_t x, y;
    VECTOR *pMVBuffPred[3];
    VECTOR *pMVBuff_temp;
    const int16_t mb_width = dec->mb_width;
    const int16_t mb_height = dec->mb_height;

    static uint8_t *block[3];
    UINT16 gGmC1PtFlag = 0;
#ifndef SHEEN_VC_DEBUG
    uint16 fillAudMbScale=MBScale4AudRed;//big than qcif
#endif

#ifndef VOC_DO
    int32_t temp_code;
    int16_t *pCoeffBuffRow;
    int16_t *pCoeffBuffCol;
    //MACROBLOCK *last_mb;
    int16_t *pLast_mbs_mode;
    VECTOR4 *pLast_mbs_mv;
    static DECMBINTER_PTR dec_mb[3] = {dec_mb_inter1v,dec_mb_inter1v,dec_mb_inter4v};
    //int temp,j;
    //short *test5;
    //int8 sendVocReq;
#else
    int temp1;
    UINT32 cs_status;
#endif

//totc=totv=0;//test
    if(mpeg4_lowres!=0)
    {
        if (!dec->is_edged[0])
        {
            image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
                           dec->width>>mpeg4_lowres, dec->height>>mpeg4_lowres, dec->bs_version,mpeg4_lowres);
            dec->is_edged[0] = 1;
        }
    }
    else
    {
        if (!dec->is_edged[0])
        {
            image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
                           dec->width, dec->height, dec->bs_version,0);
            dec->is_edged[0] = 1;
        }
    }
    if (gmc_warp)
    {
        /* accuracy: 0==1/2, 1=1/4, 2=1/8, 3=1/16 */
        gGmC1PtFlag = generate_GMCparameters( dec->sprite_warping_points,
                                              dec->sprite_warping_accuracy, gmc_warp,
                                              dec->width, dec->height, &dec->new_gmc_data);

        /* image warping is done block-based in decoder_mbgmc(), now */
    }
    bound = 0;
    block[0] = (uint8_t *)dec->block;
    block[1] = (uint8_t *)(dec->block + 256);
    block[2] = (uint8_t *)(dec->block + 320);

#ifdef longyl
    for(x = -1; x < mb_width; x++)
    {
        *(dec->ac_pred_above_y + (x<<4)) = -1;
        *(dec->ac_pred_above_y + (x<<4) +8) = -1;
        *(dec->ac_pred_above_u + (x<<3)) = -1;
        *(dec->ac_pred_above_v + (x<<3)) = -1;
    }
    *(dec->ac_pred_left_y) = -1;
    *(dec->ac_pred_left_y + 8) = -1;
    *(dec->ac_pred_left_u) = -1;
    *(dec->ac_pred_left_v) = -1;
    pMVBuff_temp = dec->mv_buffer_base+1;
    (pMVBuff_temp++)->x = 0x80000000;
    pMVBuff_temp ++;
    (pMVBuff_temp++)->x = 0x80000000;
    for(x = dec->mb_width+1; x > 0; x--)
    {
        pMVBuff_temp += 2;
        (pMVBuff_temp++)->x = 0x80000000;
        (pMVBuff_temp++)->x = 0x80000000;
    }
    pMVBuffPred[0] = dec->mv_buffer_base; //left
    dec->y_row_start = dec->cur.y;
    dec->u_row_start = dec->cur.u;
    dec->v_row_start = dec->cur.v;
#endif
///////////////////////////////////////////

    for (y = 0; y < mb_height; y++)
    {
        cp_mb = st_mb = 0;
        for (x = 0; x < mb_width; x++)
        {

#ifdef VOC_DO

            temp1 = 4- ((INT32)bs->tail & 0x3);
            if (temp1 != 4)
            {
                if(((bs->pos>>3)+1 +temp1) > 4)
                {
                    bs->buf = (bs->buf) >>((4-temp1)<<3);
                    bs->pos = bs->pos - ((4-temp1)<<3);
                    bs->tail = bs->tail - (4-temp1);
                }
                else
                {
                    while(temp1>0)
                    {
                        _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
                        temp1--;
                    }
                }
            }

#ifndef SHEEN_VC_DEBUG

            while(*(vocWorkState.pVOC_WORK) ==1/*audio*/ ||(*(vocWorkState.pVOC_WORK) ==2/*video*/ && *(vocWorkState.pVOC_VID) !=0))
            {
                //wait last voc vid done
                //diag_printf("3 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                //sxr_Sleep(1);
            }

            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=2;//video
            *(vocWorkState.pVOC_VID)=2;//more voc work to do

            hal_SysExitCriticalSection(cs_status);
#endif

//tt1=hal_TimGetUpTime();

            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_MB_DEC_ID_ADDR)))= 1;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_x_pos_ADDR)))= ((INT32)(x))|(((INT32)(y))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_width_ADDR)))= ((INT32)(dec->width))|(((INT32)(dec->height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_edged_width_ADDR)))= ((INT32)(dec->edged_width))|(((INT32)(dec->edged_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_mb_width_ADDR)))= ((INT32)(dec->mb_width))|(((INT32)(dec->mb_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_stride_u_ADDR)))= ((INT32)(dec->stride[4]))|(((INT32)(dec->stride[5]))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_fcode_ADDR)))= ((INT32)(fcode))|(((INT32)(rounding))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn0y_ADDR)))= ((INT32) dec->refn[0].y &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn0u_ADDR)))= ((INT32) dec->refn[0].u &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn0v_ADDR)))= ((INT32) dec->refn[0].v &VOC_CFG_DMA_EADDR_MASK)>>2;

            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_bs_tail_ADDR)))= ((INT32)(0))|(((INT32)(quant))<<16);//quant
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_buf_EXTADDR)))= (INT32)bs->buf;
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_pos_ADDR)))= (INT16)bs->pos;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_tail_EXTADDR)))= ((INT32) bs->tail &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_y_row_start_EXTADDR)))= (INT32)(dec->y_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_u_row_start_EXTADDR)))= (INT32)(dec->u_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_v_row_start_EXTADDR)))= (INT32)(dec->v_row_start) &VOC_CFG_DMA_EADDR_MASK;
            /*asp*/
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_quarterpel_ADDR)))=(INT16)dec->quarterpel;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_last_mb_mode_EADDR)))= (INT32)(dec->last_mbs_mode) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_last_mb_mvs_EADDR)))= (INT32)(dec->last_mbs_mv) &VOC_CFG_DMA_EADDR_MASK;

            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_intra_dc_threshold_ADDR)))= intra_dc_threshold;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_bound_ADDR)))= bound;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_DMA_STATUS_ADDR)))=((INT32)mpeg4_lowres)<<16;//GLOBAL_MPEG4_BS_DMA_flag_ADDR is unused;THE HIGH ADDRESS IS GLOBAL_MPEG4_DEC_lowres_ADDR

            if (gmc_warp)
            {

                *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_warp)))= (INT16)gmc_warp;
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_num_wp)))= ((INT32)(dec->new_gmc_data.num_wp))|(((INT32)(dec->new_gmc_data.accuracy))<<16);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_sW)))=  ((INT32)(dec->new_gmc_data.sW))|(((INT32)(dec->new_gmc_data.sH))<<16);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_dU)))=((INT32)dec->new_gmc_data.dU[0]);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_dU+2)))=((INT32)dec->new_gmc_data.dU[1]);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_dV)))=((INT32)dec->new_gmc_data.dV[0]);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_dV+2)))=((INT32)dec->new_gmc_data.dV[1]);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_Uo)))=((INT32)dec->new_gmc_data.Uo);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_Vo)))=((INT32)dec->new_gmc_data.Vo);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_Uco)))=((INT32)dec->new_gmc_data.Uco);
                *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_Vco)))=((INT32)dec->new_gmc_data.Vco);
                *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_1Pt_FlAG)))= (INT16)gGmC1PtFlag;
            }
            else
            {
                *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_GMC_warp)))= 0x00;
            }
#ifdef RAM_DCT3D
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct0>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)(DCT3D+4096) &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct1>>2;
#else
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D[0] &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct0>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)DCT3D[1] &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct1>>2;
#endif
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_mv_buffer_base_EXTADDR)))= ((INT32)dec->mv_buffer_base &VOC_CFG_DMA_EADDR_MASK)>>2;//400 int

            {
                INT32 *p1,*p2;
                INT32 i,size;

                *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_pQpBuffcurrent_ADDR)))= ((INT32)(*dec->qscale_current))|(((INT32)(*dec->qscale_left))<<16);
                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_pQpBuffAbove_ADDR));
                p2= (INT32*) (dec->qscale_above);
                size= (dec->mb_width)>>1;
                for(i=0; i<size; i++)
                    *p1++=*p2++;
                if(dec->mb_width&0x1)
                    *(INT16*)p1= *(INT16*)p2;


                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_mvs_ADDR));
                p2= (INT32*) (dec->mvs);
                for(i=0; i<8; i++)
                    *p1++=*p2++;

                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_ac_pred_base_ADDR));
                p2= (INT32*) (dec->ac_pred_above_y -8);
                size= (dec->mb_width+2) << 4;//(sizeof(int16_t) * (dec->mb_width+2)) << 5;
                for(i=0; i<size; i++)
                    *p1++=*p2++;
            }

//tt2=hal_TimGetUpTime();
// totc+=tt2- tt1;

            //CS_mpeg4_mbdec();
#ifndef SHEEN_VC_DEBUG

            vpp_AVDecSetMode(MMC_MPEG4_DECODE, 0, 0);
            hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

            //sendVocReq =0;
            while(*(vocWorkState.pVOC_VID) !=0)
            {
                //wait last voc vid done
                temp1=0;
                //start unblock display
                if(vocWorkState.vid_display)
                {
                    vocWorkState.vid_display();
                    vocWorkState.vid_display= NULL;
                }
                //audio decode first
                if(vocWorkState.aud_nframe_dec)
                    temp1=vocWorkState.aud_nframe_dec();
                //display second
                /*if(temp1!=1 && vocWorkState.vid_display)
                {
                    vocWorkState.vid_display();
                    vocWorkState.vid_display= NULL;
                    //sxr_Sleep(1);
                }*/
                // read next frame
                if(temp1!=1 && vocWorkState.vid_frame_read)
                {
                    vocWorkState.vid_frame_read();
                    vocWorkState.vid_frame_read= NULL;
                }

                //if(*(vocWorkState.pVOC_AUD) >0 && !sendVocReq)
                //{
                //INT16 *p;
                //p= (INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_QUIT_VOC_ADDR -1));
                //*(p+1)= 1;
                //  sendVocReq=1;
                //}
                //diag_printf("4 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                sxr_Sleep(1);
            }

            vocWorkState.aud_nframe_dec=NULL;
            //aac will cover these ram below.!!!!!!!!!!!!!

#else
            /*
            VoC_set_event(CFG_WAKEUP_SOF0);

#ifdef BINARY_SIM
            VoC_execute(0);//offset 0x00 =Coolsand_AudioDecode_MAIN
#else
            Coolsand_AudioDecode_MAIN();
#endif
            */
            voc_codec_exe();
#endif

//tt1=hal_TimGetUpTime();
// totv+=tt1- tt2;

            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_tail_EXTADDR);//voc reuse MPEG4_bs_tail_EXTADDR to store lo16 pos
            bs->pos = (uint16)(bs->buf & 0xffff);//lo16 pos
            bs->tail += *(UINT32*) hal_VocGetPointer(MPEG4_bs_consume_ADDR);//reuse MPEG4_bs_consume_ADDR store size consume.
            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_buf_EXTADDR);//voc reuse MPEG4_bs_buf_EXTADDR to store buf

            {
                INT32 *p1,*p2;
                INT32 i;
                UINT32 tempval;

                p2=((INT32*) hal_VocGetPointer(GLOBAL_mv_buffer_base_ADDR));
                p1= (INT32*)(dec->mv_buffer_base);//max mb_width=45 in voc.
                for(i=0; i<(sizeof(VECTOR) * (dec->mb_width + 2)); i++)
                {
                    *p1++ =*p2++;
                }

                p2= (INT32*) hal_VocGetPointer(GLOBAL_mvs_ADDR);
                p1= (INT32*) (dec->mvs);
                for(i=0; i<8; i++)
                    *p1++ =*p2++;

                p2= (INT32*) hal_VocGetPointer(GLOBAL_MPEG4_ac_pred_base_ADDR);
                p1= (INT32*) (dec->ac_pred_above_y -8);
                tempval= (dec->mb_width+2) << 4;//(sizeof(int16_t) * (dec->mb_width+2)) << 5;
                for(i=0; i<tempval; i++)
                    *p1++=*p2++;

                fcode= *((INT32*) hal_VocGetPointer(MPEG4_fcode_ADDR));
                intra_dc_threshold= *((INT32*) hal_VocGetPointer(GLOBAL_intra_dc_threshold_ADDR));
                bound= *((INT32*) hal_VocGetPointer(GLOBAL_bound_ADDR));

                quant= *((INT32*) hal_VocGetPointer(GLOBAL_MPEG4_bs_tail_ADDR));//hi16= quant
                quant>>=16;
                *(dec->qscale_left)= (*((INT32*) hal_VocGetPointer(GLOBAL_pQpBuffcurrent_ADDR)))>>16;//hi16 qscale_left

                p2= (INT32*) hal_VocGetPointer(GLOBAL_pQpBuffAbove_ADDR);
                p1= (INT32*) (dec->qscale_above);
                tempval= (dec->mb_width)>>1;
                for(i=0; i<tempval; i++)
                    *p1++=*p2++;
                if(dec->mb_width&0x1)
                    *(INT16*)p1= *(INT16*)p2;

                tempval= *((UINT32*) hal_VocGetPointer(MPEG4_x_pos_ADDR));
                x= tempval &0xFFFF;
                y= tempval>>16;
                if(mpeg4_lowres==1)
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<3)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<1)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<1)*y;

                }
                else
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<4)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<2)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<2)*y;
                }

            }

#ifndef SHEEN_VC_DEBUG
            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=0;//video
            if( y <(mb_height -1) || x< (mb_width -1))
                *(vocWorkState.pVOC_VID)=1;//more voc work to do
            //if(vocWorkState.vid_voc_isr)
            //  vocWorkState.vid_voc_isr();

            hal_SysExitCriticalSection(cs_status);
#endif

//tt2=hal_TimGetUpTime();
//totc+=tt2- tt1;

#else //VOC_DO


            /*    if(macroblock_number == 50)
                  {
                    macroblock_number = 0;
                  }*/
            pMVBuffPred[1] = pMVBuffPred[0] + (x<<2) + 4; //top
            pMVBuffPred[2] = pMVBuffPred[1] + 4;  //topright
            //last_mb = &dec->last_mbs[y * dec->mb_width + x];
            pLast_mbs_mode = &dec->last_mbs_mode[y * dec->mb_width + x];
            pLast_mbs_mv = &dec->last_mbs_mv[y * dec->mb_width + x];
            /* skip stuffing */
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _SHOWBITS(bs->buf,bs->pos,10,temp_code);
#if 0
            while (temp_code == 1)
                bs->pos -= 10;
#else
            while(temp_code == 1)
            {
                bs->pos -= 10;
                if(bs->pos < 16)//sheen
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _SHOWBITS(bs->buf,bs->pos,10,temp_code);
            }
#endif
            if (check_resync_marker(bs, fcode - 1))
            {
                int16 tempx;//added bi liyongjian
                bound = read_video_packet_header(bs, dec, fcode - 1,
                                                 &quant, &fcode, NULL, &intra_dc_threshold);
                //x = bound % mb_width; //avoid Avatar.mp4 crash
                //y = bound / mb_width;
                //add by longyl
#ifdef longyl
                for(tempx = -1; tempx < mb_width; tempx ++)
                {
                    *(dec->ac_pred_above_y + (tempx<<4)) = -1;
                    *(dec->ac_pred_above_y + (tempx<<4) +8) = -1;
                    *(dec->ac_pred_above_u + (tempx<<3)) = -1;
                    *(dec->ac_pred_above_v + (tempx<<3)) = -1;
                }
                *(dec->ac_pred_left_y) = -1;
                *(dec->ac_pred_left_y + 8) = -1;
                *(dec->ac_pred_left_u) = -1;
                *(dec->ac_pred_left_v) = -1;

                pMVBuff_temp = dec->mv_buffer_base+1;
                (pMVBuff_temp++)->x = 0x80000000;
                pMVBuff_temp ++;
                (pMVBuff_temp++)->x = 0x80000000;
                for(tempx = dec->mb_width+1; tempx > 0; tempx--)
                {
                    pMVBuff_temp += 2;
                    (pMVBuff_temp++)->x = 0x80000000;
                    (pMVBuff_temp++)->x = 0x80000000;
                }
                pMVBuffPred[0] = dec->mv_buffer_base; //left
#endif
///////////////////////////////////////////
            }

            if(bs->pos < 16)//sheen
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }

            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code)   /* block _is_ coded */
            {
                uint16_t mcbpc, cbpc, cbpy, cbp;
                int16_t intra;
                int16_t acpred_flag = 0;
                int mcsel = 0;    /* mcsel: '0'=local motion, '1'=GMC *///add yh;
                cp_mb++;
                {
                    uint16_t index;

                    _SHOWBITS(bs->buf,bs->pos,9,temp_code)
                    index = MIN_32C(256,temp_code);
                    bs->pos -= mcbpc_inter_table[index].len;
                    mcbpc = mcbpc_inter_table[index].code;
                }

                dec->mode = mcbpc & 7;
                //last_mb->mode = dec->mode;
                *pLast_mbs_mode = dec->mode;
                cbpc = (mcbpc >> 4);

                intra = (dec->mode == MODE_INTRA || dec->mode == MODE_INTRA_Q);

                if (gmc_warp && (dec->mode == MODE_INTER || dec->mode == MODE_INTER_Q))
                {
                    _GETBITS(bs->buf,bs->pos,1,mcsel)
                }
                else if(intra)
                    _GETBITS(bs->buf,bs->pos,1,acpred_flag);
                {
                    uint16_t index;
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _SHOWBITS(bs->buf,bs->pos,6,index);
                    bs->pos -= cbpy_table[index].len;
                    cbpy = cbpy_table[index].code+ (((intra-1)>>15)&(15-(cbpy_table[index].code << 1)));
                }
                cbp = (cbpy << 2) | cbpc;
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                if (dec->mode == MODE_INTER_Q || dec->mode == MODE_INTRA_Q)
                {
                    _GETBITS(bs->buf,bs->pos,2,temp_code);
                    quant += dquant_table[temp_code];
//        quant = 31 + (((quant-31)>>31)&(quant-31));
//        quant = 1 + (((1-quant)>>31)&(1-quant));
                    if (quant > 31)
                    {
                        quant = 31;
                    }
                    else if (quant < 1)
                    {
                        quant = 1;
                    }
                }
                *dec->qscale_current = quant;
                if (mcsel)
                {
                    decoder_mbgmc(dec,pMVBuffPred, block, x, y, fcode, cbp, bs, rounding);
                    //last_mb->mvs[0] = dec->mvs[0];
                    //last_mb->mvs[1] = dec->mvs[1];
                    //last_mb->mvs[2] = dec->mvs[2];
                    //last_mb->mvs[3] = dec->mvs[3];
                    pLast_mbs_mv->mvs[0] = dec->mvs[0];
                    pLast_mbs_mv->mvs[1] = dec->mvs[1];
                    pLast_mbs_mv->mvs[2] = dec->mvs[2];
                    pLast_mbs_mv->mvs[3] = dec->mvs[3];
                    pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                    pCoeffBuffCol = dec->ac_pred_left_y;

                    *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                    *pCoeffBuffRow = -1;

                    *pCoeffBuffCol = -1;
                    *(pCoeffBuffCol + 8) = -1;

                    pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_u;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;

                    pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_v;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;
                    continue;

                }
                else if (dec->mode < 3)
                {
                    dec_mb[dec->mode](dec,pMVBuffPred,block,x,y,cbp,bs,fcode,rounding);
                    //last_mb->mvs[0] = dec->mvs[0];
                    //last_mb->mvs[1] = dec->mvs[1];
                    //last_mb->mvs[2] = dec->mvs[2];
                    //last_mb->mvs[3] = dec->mvs[3];
                    pLast_mbs_mv->mvs[0] = dec->mvs[0];
                    pLast_mbs_mv->mvs[1] = dec->mvs[1];
                    pLast_mbs_mv->mvs[2] = dec->mvs[2];
                    pLast_mbs_mv->mvs[3] = dec->mvs[3];

                    pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                    pCoeffBuffCol = dec->ac_pred_left_y;

                    *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                    *pCoeffBuffRow = -1;

                    *pCoeffBuffCol = -1;
                    *(pCoeffBuffCol + 8) = -1;

                    pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_u;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;

                    pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_v;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;
                }
                else   /* MODE_INTRA, MODE_INTRA_Q */
                {
                    pMVBuffPred[1][2].x = pMVBuffPred[1][3].x = pMVBuffPred[0][1].x
                                          = pMVBuffPred[0][3].x = dec->mvs[0].x = 0;
                    pMVBuffPred[1][2].y = pMVBuffPred[1][3].y = pMVBuffPred[0][1].y
                                          = pMVBuffPred[0][3].y = dec->mvs[0].y = 0;

                    //last_mb->mvs[3] = last_mb->mvs[2]
                    //  = last_mb->mvs[1] = last_mb->mvs[0] = dec->mvs[0];
                    pLast_mbs_mv->mvs[3] = pLast_mbs_mv->mvs[2]
                                           = pLast_mbs_mv->mvs[1] = pLast_mbs_mv->mvs[0] = dec->mvs[0];

                    decoder_mbintra(dec, x, y, acpred_flag, cbp, bs, quant,
                                    intra_dc_threshold, bound);
                }
            }
            else if (gmc_warp)     /* a not coded S(GMC)-VOP macroblock */
            {
                dec->mode = MODE_NOT_CODED_GMC;
                //last_mb->mode = dec->mode;
                *pLast_mbs_mode = dec->mode;
                *dec->qscale_current = quant;
                decoder_mbgmc(dec, pMVBuffPred, block,x, y, fcode, 0x00, bs, rounding);

                if(dec->out_frm && cp_mb > 0)
                {
                    //output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
                    cp_mb = 0;
                }
                st_mb = x+1;
                //last_mb->mvs[0] = dec->mvs[0];
                //last_mb->mvs[1] = dec->mvs[1];
                //last_mb->mvs[2] = dec->mvs[2];
                //last_mb->mvs[3] = dec->mvs[3];
                pLast_mbs_mv->mvs[0] = dec->mvs[0];
                pLast_mbs_mv->mvs[1] = dec->mvs[1];
                pLast_mbs_mv->mvs[2] = dec->mvs[2];
                pLast_mbs_mv->mvs[3] = dec->mvs[3];

                pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                pCoeffBuffCol = dec->ac_pred_left_y;

                *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                *pCoeffBuffRow = -1;

                *pCoeffBuffCol = -1;
                *(pCoeffBuffCol + 8) = -1;

                pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_u;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;

                pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_v;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;
            }
            else
            {
                /* not coded P_VOP macroblock */
                int32_t y_add_temp,uv_add_temp,i;

                dec->mode = MODE_NOT_CODED;
                //last_mb->mode = dec->mode;
                *pLast_mbs_mode = dec->mode;

                *(dec->qscale_above+ x) =  *dec->qscale_left = *dec->qscale_current = quant;

                pMVBuffPred[1][2].x = pMVBuffPred[1][3].x = pMVBuffPred[0][1].x
                                      = pMVBuffPred[0][3].x = dec->mvs[0].x = 0;
                pMVBuffPred[1][2].y = pMVBuffPred[1][3].y = pMVBuffPred[0][1].y
                                      = pMVBuffPred[0][3].y = dec->mvs[0].y = 0;
                //last_mb->mvs[3] = last_mb->mvs[2]
                //  = last_mb->mvs[1] = last_mb->mvs[0] = dec->mvs[0];
                pLast_mbs_mv->mvs[3] = pLast_mbs_mv->mvs[2]
                                       = pLast_mbs_mv->mvs[1] = pLast_mbs_mv->mvs[0] = dec->mvs[0];


                if(mpeg4_lowres == 1)
                {
                    int h = 8;//block * 2;
                    int sx = (dec->mvs[0].x << 2) >> mpeg4_lowres;
                    int sy = (dec->mvs[0].y << 2) >> mpeg4_lowres;
                    UINT8 *pY_Cur = dec->y_row_start + (x<<3);
                    UINT8 *pU_Cur = dec->u_row_start + (x<<2);
                    UINT8 *pV_Cur = dec->v_row_start + (x<<2);
                    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+(x << 3);
                    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x << 2);

                    put_mpeg4_chroma_mc8_8_c(pY_Cur, dec->refn[0].y + y_add_temp, dec->stride[0], h, sx, sy);
                    put_mpeg4_chroma_mc4_8_c(pU_Cur, dec->refn[0].u + uv_add_temp, dec->stride[4], h >>1, sx>>1, sy>>1);
                    put_mpeg4_chroma_mc4_8_c(pV_Cur, dec->refn[0].v + uv_add_temp, dec->stride[4], h >> 1, sx>>1, sy>>1);

                }
                else
                {
                    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+(x << 4);
                    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x << 3);
                    interpolate16x16_switch(block[0], dec->refn[0].y + y_add_temp,
                                            0, 0, dec->edged_width, rounding);
                    interpolate8x8_switch(block[1], dec->refn[0].u + uv_add_temp, 0, 0, (dec->edged_width >> 1), rounding);
                    interpolate8x8_switch(block[2], dec->refn[0].v + uv_add_temp, 0, 0, (dec->edged_width >> 1), rounding);
                    //y
                    {
                        uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
                        uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.y + y_add_temp);
                        uint32_t stride_tmp = (dec->edged_width >> 2);
                        uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
                        for(i = 8; i > 0; i--)
                        {
                            dst_addr_tmp[0] = ref_addr_tmp0[0];
                            dst_addr_tmp[1] = ref_addr_tmp0[1];
                            dst_addr_tmp[2] = ref_addr_tmp0[16];
                            dst_addr_tmp[3] = ref_addr_tmp0[17];

                            dst_addr_stride[0] = ref_addr_tmp0[32];
                            dst_addr_stride[1] = ref_addr_tmp0[33];
                            dst_addr_stride[2] = ref_addr_tmp0[48];
                            dst_addr_stride[3] = ref_addr_tmp0[49];

                            dst_addr_tmp += stride_tmp;
                            dst_addr_stride += stride_tmp;
                            ref_addr_tmp0 += 2;
                        }
                    }
                    //u
                    {
                        uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
                        uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.u + uv_add_temp);
                        uint32_t stride_tmp = (dec->edged_width >> 3);

                        for(i = 8; i > 0; i--)
                        {
                            dst_addr_tmp[0] = ref_addr_tmp0[0];
                            dst_addr_tmp[1] = ref_addr_tmp0[1];

                            dst_addr_tmp += stride_tmp;
                            ref_addr_tmp0 += 2;
                        }
                    }
                    //v
                    {
                        uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
                        uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.v + uv_add_temp);
                        uint32_t stride_tmp = (dec->edged_width >> 3);

                        for(i = 8; i > 0; i--)
                        {
                            dst_addr_tmp[0] = ref_addr_tmp0[0];
                            dst_addr_tmp[1] = ref_addr_tmp0[1];

                            dst_addr_tmp += stride_tmp;
                            ref_addr_tmp0 += 2;
                        }
                    }
                }
                pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                pCoeffBuffCol = dec->ac_pred_left_y;

                *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                *pCoeffBuffRow = -1;

                *pCoeffBuffCol = -1;
                *(pCoeffBuffCol + 8) = -1;

                pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_u;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;

                pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_v;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;

                /*      if(dec->out_frm && cp_mb > 0) {
                        output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
                        cp_mb = 0;
                      }
                      st_mb = x+1;*/


            }

#endif //VOC_DO

        }
#ifdef longyl
        *(dec->ac_pred_above_y + (mb_width<<4) - 8) = *(dec->ac_pred_left_y + 8);
        *(dec->ac_pred_above_u + (mb_width<<3) - 8) = *(dec->ac_pred_left_u);
        *(dec->ac_pred_above_v + (mb_width<<3) - 8) = *(dec->ac_pred_left_v);

        *(dec->ac_pred_left_y) = -1;
        *(dec->ac_pred_left_y + 8) = -1;
        *(dec->ac_pred_left_u) = -1;
        *(dec->ac_pred_left_v) = -1;
        pMVBuffPred[0][1].x = pMVBuffPred[0][3].x = 0x80000000;

        if(mpeg4_lowres==1)
        {
            dec->y_row_start += dec->edged_width << 3;
            dec->u_row_start += dec->edged_width << 1;
            dec->v_row_start += dec->edged_width << 1;
        }
        else
        {
            dec->y_row_start += dec->edged_width<<4;
            dec->u_row_start += dec->edged_width<<2;
            dec->v_row_start += dec->edged_width<<2;
        }
#endif

#ifndef SHEEN_VC_DEBUG
#ifndef MEDIA_VOCVID_SUPPORT
        if(fillAudioData && (y*mb_width)>=fillAudMbScale)//read audio when image big than qcif
        {
            fillAudioData();
            fillAudMbScale+=MBScale4AudRed;
        }
#endif
#endif


        /*    if(dec->out_frm && cp_mb > 0)
              output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);*/
    }

//diag_printf("p totc %d totv %d", totc*1000/16384, totv*1000/16384);

}


static int get_resync_len_b(const int fcode_backward,
                            const int fcode_forward)
{
    int resync_len = ((fcode_forward>fcode_backward) ? fcode_forward : fcode_backward) - 1;
    if (resync_len < 1) resync_len = 1;
    return resync_len;
}


static int32_t
get_mbtype(Bitstream * bs)
{
    int32_t mb_type,temp_code;

    for (mb_type = 0; mb_type <= 3; mb_type++)
    {
        _GETBITS(bs->buf,bs->pos,1,temp_code)
        if (temp_code)
            return (mb_type);
    }
    return -1;
}

static  int32_t
get_dbquant(Bitstream * bs)
{
    int32_t temp_code;

    _GETBITS(bs->buf,bs->pos,1,temp_code)
    if (!temp_code)
    {
        /*  '0' */
        return (0);
        _GETBITS(bs->buf,bs->pos,1,temp_code)
    }
    else if(!temp_code)
    {
        /* '10' */
        return (-2);
    }
    else              /* '11' */
        return (2);
}

void
decoder_bframe(DECODER * dec,
               Bitstream * bs,
               int quant,
               int fcode_forward,
               int fcode_backward)
{
    uint32_t x, y;
    VECTOR mv;
    const VECTOR zeromv = {0,0};
    int i;
    int resync_len;


    uint16_t bound;
    //  int cp_mb, st_mb;
    VECTOR F_MVBuffPred;
    VECTOR B_MVBuffPred;

    const int16_t mb_width = dec->mb_width;
    const int16_t mb_height = dec->mb_height;

    static uint8_t *block[3];
#ifndef SHEEN_VC_DEBUG
    uint16 fillAudMbScale=MBScale4AudRed;//big than qcif
#endif
#ifndef VOC_DO
    int32_t temp_code;
    int16_t *pCoeffBuffRow;
    int16_t *pCoeffBuffCol;
    //int temp,j;
    //short *test5;
    //int8 sendVocReq;
#else
    int temp1;
    UINT32 cs_status;
#endif

    if (!dec->is_edged[0])
    {
        image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
                       dec->width, dec->height, dec->bs_version,0);
        dec->is_edged[0] = 1;
    }

    if (!dec->is_edged[1])
    {
        image_setedges(&dec->refn[1], dec->edged_width, dec->edged_height,
                       dec->width, dec->height, dec->bs_version,0);
        dec->is_edged[1] = 1;
    }


    bound = 0;
    block[0] = (uint8_t *)dec->block;
    block[1] = (uint8_t *)(dec->block + 256);
    block[2] = (uint8_t *)(dec->block + 320);

#ifdef longyl

    dec->y_row_start = dec->cur.y;
    dec->u_row_start = dec->cur.u;
    dec->v_row_start = dec->cur.v;
#endif

    resync_len = get_resync_len_b(fcode_backward, fcode_forward);

    for (y = 0; y < dec->mb_height; y++)
    {
        /* Initialize Pred Motion Vector */
        //dec->p_fmv = dec->p_bmv = zeromv;
        F_MVBuffPred = zeromv;
        B_MVBuffPred = zeromv;
        for (x = 0; x < dec->mb_width; x++)
        {

#ifdef VOC_DO

            temp1 = 4- ((INT32)bs->tail & 0x3);
            if (temp1 != 4)
            {
                if(((bs->pos>>3)+1 +temp1) > 4)
                {
                    bs->buf = (bs->buf) >>((4-temp1)<<3);
                    bs->pos = bs->pos - ((4-temp1)<<3);
                    bs->tail = bs->tail - (4-temp1);
                }
                else
                {
                    while(temp1>0)
                    {
                        _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
                        temp1--;
                    }
                }
            }

#ifndef SHEEN_VC_DEBUG

            while(*(vocWorkState.pVOC_WORK) ==1/*audio*/ ||(*(vocWorkState.pVOC_WORK) ==2/*video*/ && *(vocWorkState.pVOC_VID) !=0))
            {
                //wait last voc vid done
                //diag_printf("3 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                //sxr_Sleep(1);
            }

            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=2;//video
            *(vocWorkState.pVOC_VID)=2;//more voc work to do

            hal_SysExitCriticalSection(cs_status);
#endif

            //tt1=hal_TimGetUpTime();

            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_MB_DEC_ID_ADDR)))= 2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_x_pos_ADDR)))= ((INT32)(x))|(((INT32)(y))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_width_ADDR)))= ((INT32)(dec->width))|(((INT32)(dec->height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_edged_width_ADDR)))= ((INT32)(dec->edged_width))|(((INT32)(dec->edged_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_mb_width_ADDR)))= ((INT32)(dec->mb_width))|(((INT32)(dec->mb_height))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_stride_u_ADDR)))= ((INT32)(dec->stride[4]))|(((INT32)(dec->stride[5]))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_fcode_ADDR)))= ((INT32)(0)<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_fcode_forward)))= ((INT32)(fcode_forward))|(((INT32)(fcode_backward))<<16);
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_b_y_ADDR)))= ((INT32) dec->refn[0].y &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_b_u_ADDR)))= ((INT32) dec->refn[0].u &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_b_v_ADDR)))= ((INT32) dec->refn[0].v &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_f_y_ADDR)))= ((INT32) dec->refn[1].y &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_f_u_ADDR)))= ((INT32) dec->refn[1].u &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_refn_f_v_ADDR)))= ((INT32) dec->refn[1].v &VOC_CFG_DMA_EADDR_MASK)>>2;

            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_bs_tail_ADDR)))= ((INT32)(0))|(((INT32)(quant))<<16);//quant
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_buf_EXTADDR)))= (INT32)bs->buf;
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_pos_ADDR)))= (INT16)bs->pos;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_bs_tail_EXTADDR)))= ((INT32) bs->tail &VOC_CFG_DMA_EADDR_MASK)>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_y_row_start_EXTADDR)))= (INT32)(dec->y_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_u_row_start_EXTADDR)))= (INT32)(dec->u_row_start) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_v_row_start_EXTADDR)))= (INT32)(dec->v_row_start) &VOC_CFG_DMA_EADDR_MASK;

            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_bound_ADDR)))= bound;//for resync;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_MPEG4_DMA_STATUS_ADDR)))=((INT32)mpeg4_lowres)<<16;//THE HIGH ADDRESS IS GLOBAL_MPEG4_DEC_lowres_ADDR;

            /*asp*/
            *((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_quarterpel_ADDR)))=(INT16)dec->quarterpel;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_last_mb_mode_EADDR)))= (INT32)(dec->last_mbs_mode) &VOC_CFG_DMA_EADDR_MASK;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_last_mb_mvs_EADDR)))= (INT32)(dec->last_mbs_mv) &VOC_CFG_DMA_EADDR_MASK;
            *(((INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_ASP_resync_len_ADDR)))+MPEG4_DEC_ASP_resync_len_ADDR%2)=(INT16)resync_len;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(MPEG4_DEC_time_bp)))= ((INT32)(dec->time_bp))|(((INT32)(dec->time_pp))<<16);

#if 0
#ifdef RAM_DCT3D
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct0>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)(DCT3D+4096) &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct1>>2;
#else
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT0_ADDR)))= ((INT32)DCT3D[0] &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct0>>2;
            *((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_DCT1_ADDR)))= ((INT32)DCT3D[1] &VOC_CFG_DMA_EADDR_MASK)>>2;//(INT32)dct1>>2;
#endif
#endif

            //*((INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_mv_buffer_base_EXTADDR)))= ((INT32)dec->mv_buffer_base &VOC_CFG_DMA_EADDR_MASK)>>2;//400 int

            {
                INT32 *p1,*p2;
                INT32 i;

                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_mvs_ADDR));
                p2= (INT32*) (&F_MVBuffPred);
                for(i=0; i<2; i++)
                    *p1++=*p2++;

                p1= (INT32*) NoCacheAddr(hal_VocGetPointer(GLOBAL_bmvs_ADDR));
                p2= (INT32*) (&B_MVBuffPred);
                for(i=0; i<2; i++)
                    *p1++=*p2++;
            }

            //tt2=hal_TimGetUpTime();
            // totc+=tt2- tt1;

#ifndef SHEEN_VC_DEBUG

            vpp_AVDecSetMode(MMC_MPEG4_DECODE, 0, 0);
            hal_VocWakeup(HAL_VOC_WAKEUP_EVENT_0);

            //sendVocReq =0;
            while(*(vocWorkState.pVOC_VID) !=0)
            {
                //wait last voc vid done
                temp1=0;
                //start unblock display
                if(vocWorkState.vid_display)
                {
                    vocWorkState.vid_display();
                    vocWorkState.vid_display= NULL;
                }
                //audio decode first
                if(vocWorkState.aud_nframe_dec)
                    temp1=vocWorkState.aud_nframe_dec();
                //display second
                /*if(temp1!=1 && vocWorkState.vid_display)
                {
                   vocWorkState.vid_display();
                   vocWorkState.vid_display= NULL;
                   //sxr_Sleep(1);
                }*/
                // read next frame
                if(temp1!=1 && vocWorkState.vid_frame_read)
                {
                    vocWorkState.vid_frame_read();
                    vocWorkState.vid_frame_read= NULL;
                }

                //if(*(vocWorkState.pVOC_AUD) >0 && !sendVocReq)
                //{
                //INT16 *p;
                //p= (INT16*) NoCacheAddr(hal_VocGetPointer(MPEG4_QUIT_VOC_ADDR -1));
                //*(p+1)= 1;
                //  sendVocReq=1;
                //}
                //diag_printf("4 %d %d ",*(vocWorkState.pVOC_WORK), *(vocWorkState.pVOC_VID) );
                sxr_Sleep(1);
            }

            vocWorkState.aud_nframe_dec=NULL;
            //aac will cover these ram below.!!!!!!!!!!!!!


#else
            /*
            VoC_set_event(CFG_WAKEUP_SOF0);
#ifdef BINARY_SIM
            VoC_execute(0);//offset 0x00 =Coolsand_AudioDecode_MAIN
#else
            Coolsand_AudioDecode_MAIN();
#endif
            */
            voc_codec_exe();
#endif

            //tt1=hal_TimGetUpTime();
            // totv+=tt1- tt2;

            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_tail_EXTADDR);//voc reuse MPEG4_bs_tail_EXTADDR to store lo16 pos
            bs->pos = (uint16)(bs->buf & 0xffff);//lo16 pos
            bs->tail += *(UINT32*) hal_VocGetPointer(MPEG4_bs_consume_ADDR);//reuse MPEG4_bs_consume_ADDR store size consume.
            bs->buf= *(UINT32*) hal_VocGetPointer(MPEG4_bs_buf_EXTADDR);//voc reuse MPEG4_bs_buf_EXTADDR to store buf

            {
                UINT32 tempval;

                tempval= *((UINT32*) hal_VocGetPointer(MPEG4_x_pos_ADDR));
                x= tempval &0xFFFF;
                y= tempval>>16;
                if(mpeg4_lowres==1)
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<3)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<1)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<1)*y;

                }
                else
                {
                    dec->y_row_start = dec->cur.y +(dec->edged_width<<4)*y;
                    dec->u_row_start = dec->cur.u +(dec->edged_width<<2)*y;
                    dec->v_row_start = dec->cur.v +(dec->edged_width<<2)*y;
                }

            }

#ifndef SHEEN_VC_DEBUG
            cs_status=hal_SysEnterCriticalSection();

            *(vocWorkState.pVOC_WORK)=0;//video
            if( y <(mb_height -1) || x< (mb_width -1))
                *(vocWorkState.pVOC_VID)=1;//more voc work to do
            //if(vocWorkState.vid_voc_isr)
            //  vocWorkState.vid_voc_isr();

            hal_SysExitCriticalSection(cs_status);
#endif

            //tt2=hal_TimGetUpTime();
            //totc+=tt2- tt1;

#else //VOC_DO

            MACROBLOCK *mb = &dec->mbs[y * dec->mb_width + x];
            //MACROBLOCK *last_mb = &dec->last_mbs[y * dec->mb_width + x];
            int16_t *pLast_mbs_mode = &dec->last_mbs_mode[y * dec->mb_width + x];
            VECTOR4 *pLast_mbs_mv = &dec->last_mbs_mv[y * dec->mb_width + x];
            int intra_dc_threshold; /* fake variable */
            int cbp = 0;
            int field_dct = 0;
            int field_pred = 0;
            int field_for_top = 0;
            int field_for_bot = 0;

            if((x == 12) && (y==14))
            {
                while(0);
            }
            mv = zeromv;
            *dec->qscale_current = quant;

            dec->mvs[0] = dec->bmvs[0] = zeromv;

            /*
             * skip if the co-located P_VOP macroblock is not coded
             * if not codec in co-located S_VOP macroblock is _not_
             * automatically skipped
             */
            //last_mb->mode
            if (*pLast_mbs_mode == MODE_NOT_CODED)
            {
                int32_t y_add_temp,uv_add_temp,i;
                int rounding = 0;
                cbp = 0;
                dec->mode = MODE_FORWARD;

                if(mpeg4_lowres == 1)
                {
                    int h = 8;//block * 2;
                    int sx = (dec->mvs[0].x << 2) >> mpeg4_lowres;
                    int sy = (dec->mvs[0].y << 2) >> mpeg4_lowres;
                    UINT8 *pY_Cur = dec->y_row_start + (x<<3);
                    UINT8 *pU_Cur = dec->u_row_start + (x<<2);
                    UINT8 *pV_Cur = dec->v_row_start + (x<<2);
                    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+(x << 3);
                    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x << 2);

                    put_mpeg4_chroma_mc8_8_c(pY_Cur, dec->refn[1].y + y_add_temp, dec->stride[0], h, sx, sy);
                    put_mpeg4_chroma_mc4_8_c(pU_Cur, dec->refn[1].u + uv_add_temp, dec->stride[4], h >>1, sx>>1, sy>>1);
                    put_mpeg4_chroma_mc4_8_c(pV_Cur, dec->refn[1].v + uv_add_temp, dec->stride[5], h >> 1, sx>>1, sy>>1);

                }
                else
                {
                    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+(x << 4);
                    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x << 3);
                    interpolate16x16_switch(block[0], dec->refn[1].y + y_add_temp,
                                            0, 0, dec->stride[0], rounding);
                    interpolate8x8_switch(block[1], dec->refn[1].u + uv_add_temp, 0, 0, dec->stride[4], rounding);
                    interpolate8x8_switch(block[2], dec->refn[1].v + uv_add_temp, 0, 0, dec->stride[5], rounding);
                }

                {
                    uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
                    uint32_t *y_dst_addr_tmp = (uint32_t *)(dec->y_row_start + (x<<4));
                    uint32_t y_stride_tmp = (dec->edged_width >> 2);
                    uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

                    uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
                    uint32_t *u_dst_addr_tmp = (uint32_t *)(dec->u_row_start + (x<<3));
                    uint32_t uv_stride_tmp = (dec->edged_width >> 3);

                    uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
                    uint32_t *v_dst_addr_tmp = (uint32_t *)(dec->v_row_start + (x<<3));

                    for(i = 8; i > 0; i--)
                    {
                        y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
                        y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
                        y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
                        y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

                        dst_addr_stride[0] = y_ref_addr_tmp0[32];
                        dst_addr_stride[1] = y_ref_addr_tmp0[33];
                        dst_addr_stride[2] = y_ref_addr_tmp0[48];
                        dst_addr_stride[3] = y_ref_addr_tmp0[49];

                        y_dst_addr_tmp += y_stride_tmp;
                        dst_addr_stride += y_stride_tmp;
                        y_ref_addr_tmp0 += 2;

                        u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
                        u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

                        u_dst_addr_tmp += uv_stride_tmp;
                        u_ref_addr_tmp0 += 2;

                        v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
                        v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

                        v_dst_addr_tmp += uv_stride_tmp;
                        v_ref_addr_tmp0 += 2;
                    }
                }
                continue;
            }

            if (check_resync_marker(bs, resync_len))
            {
                bound = read_video_packet_header(bs, dec, resync_len, &quant,
                                                 &fcode_forward, &fcode_backward, &intra_dc_threshold);

                bound = MAX(0, bound-1); /* valid bound must always be >0 */
                x = bound % dec->mb_width;
                y = MIN((bound / dec->mb_width), (dec->mb_height-1));
                /* reset predicted macroblocks */
                //dec->p_fmv = dec->p_bmv = zeromv;
                F_MVBuffPred = B_MVBuffPred = zeromv;
                /* update resync len with new fcodes */
                resync_len = get_resync_len_b(fcode_backward, fcode_forward);
                continue; /* re-init loop */
            }

            if(bs->pos < 16)//sheen
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }

            _GETBITS(bs->buf,bs->pos,1,temp_code);

            if (!temp_code)
            {
                /* modb=='0' */
                //const uint8_t modb2 = BitstreamGetBit(bs);
                uint8_t modb2;
                _GETBITS(bs->buf,bs->pos,1,modb2);
                dec->mode = get_mbtype(bs);
                if(!modb2)   /* modb=='00' */
                {
                    //mb->cbp = BitstreamGetBits(bs, 6);
                    _GETBITS(bs->buf, bs->pos,6,cbp);
                }
                else
                    cbp = 0;

                if (dec->mode &&cbp)
                {
                    quant += get_dbquant(bs);
                    if (quant > 31)
                        quant = 31;
                    else if (quant < 1)
                        quant = 1;
                }
                //mb->quant = quant;
                *dec->qscale_current = quant;

                if(bs->pos < 16)//sheen
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }

                if (dec->interlacing)
                {
                    if (cbp)
                    {
                        //mb->field_dct = BitstreamGetBit(bs);
                        _GETBITS(bs->buf, bs->pos,1,field_dct);
                    }
                    if(dec->mode)
                    {
                        // mb->field_pred = BitstreamGetBit(bs);
                        _GETBITS(bs->buf, bs->pos,1,field_pred);
                        if (field_pred)
                        {
                            //mb->field_for_top = BitstreamGetBit(bs);
                            _GETBITS(bs->buf, bs->pos,1,field_for_top);
                            // mb->field_for_bot = BitstreamGetBit(bs);
                            _GETBITS(bs->buf, bs->pos,1, field_for_bot);
                        }
                    }
                }

            }
            else
            {
                dec->mode = MODE_DIRECT_NONE_MV;
                cbp = 0;
            }

            switch (dec->mode)
            {
                case MODE_DIRECT:
                    //get_b_motion_vector(bs, &mv, 1, zeromv, dec, x, y);

                    dec_mv(1,&mv,&zeromv,bs);

                case MODE_DIRECT_NONE_MV:
                    for (i = 0; i < 4; i++)
                    {
                        //mb->mvs[i].x = last_mb->mvs[i].x*dec->time_bp/dec->time_pp + mv.x;
                        dec->mvs[i].x = pLast_mbs_mv->mvs[i].x*dec->time_bp/dec->time_pp + mv.x;
                        //mb->mvs[i].y = last_mb->mvs[i].y*dec->time_bp/dec->time_pp + mv.y;
                        dec->mvs[i].y = pLast_mbs_mv->mvs[i].y*dec->time_bp/dec->time_pp + mv.y;

                        dec->bmvs[i].x = (mv.x)
                                         ?  dec->mvs[i].x - pLast_mbs_mv->mvs[i].x
                                         : pLast_mbs_mv->mvs[i].x*(dec->time_bp - dec->time_pp)/dec->time_pp;
                        dec->bmvs[i].y = (mv.y)
                                         ? dec->mvs[i].y - pLast_mbs_mv->mvs[i].y
                                         : pLast_mbs_mv->mvs[i].y*(dec->time_bp - dec->time_pp)/dec->time_pp;
                    }

                    decoder_bf_interpolate_mbinter(dec,block,x, y,cbp,
                                                   bs, 1);
                    break;

                case MODE_INTERPOLATE:
                    //get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, x, y);
                    dec_mv(fcode_forward,&mv,&F_MVBuffPred,bs);

                    dec->mvs[0] = dec->mvs[1] = dec->mvs[2] = dec->mvs[3] = F_MVBuffPred = mv;

                    //get_b_motion_vector(bs, &mb->b_mvs[0], fcode_backward, dec->p_bmv, dec, x, y);
                    dec_mv(fcode_backward,&mv,&B_MVBuffPred,bs);

                    dec->bmvs[0] = dec->bmvs[1] = dec->bmvs[2] = dec->bmvs[3] = B_MVBuffPred = mv;

                    decoder_bf_interpolate_mbinter(dec,block,x, y,cbp,
                                                   bs, 0);
                    break;

                case MODE_BACKWARD:
                    //get_b_motion_vector(bs, &mb->mvs[0], fcode_backward, dec->p_bmv, dec, x, y);
                    dec_mv(fcode_backward,&mv,&B_MVBuffPred,bs);

                    dec->bmvs[0] = dec->bmvs[1] = dec->bmvs[2] = dec->bmvs[3] = B_MVBuffPred = mv;

                    //decoder_mbinter(dec, mb, x, y, mb->cbp, bs, 0, 0, 1);
                    dec_bf_mb_inter1v(dec,block,x,y,cbp,
                                      bs,0,0);
                    break;

                case MODE_FORWARD:
                    //get_b_motion_vector(bs, &mb->mvs[0], fcode_forward, dec->p_fmv, dec, x, y);
                    dec_mv(fcode_forward,&mv,&F_MVBuffPred,bs);

                    dec->mvs[0] = dec->mvs[1] = dec->mvs[2] = dec->mvs[3] = F_MVBuffPred = mv;

                    //decoder_mbinter(dec, mb, x, y, mb->cbp, bs, 0, 1, 1);
                    dec_bf_mb_inter1v(dec,block,x,y,cbp,
                                      bs,0,1);
                    break;

                default:
                    DPRINTF(XVID_DEBUG_ERROR,"Not supported B-frame mb_type = %i\n", dec->mode);
            }
#endif //VOC_DO

        } /* End of for */
        if(mpeg4_lowres==1)
        {
            dec->y_row_start += dec->edged_width << 3;
            dec->u_row_start += dec->edged_width << 1;
            dec->v_row_start += dec->edged_width << 1;
        }
        else
        {
            dec->y_row_start += dec->edged_width<<4;
            dec->u_row_start += dec->edged_width<<2;
            dec->v_row_start += dec->edged_width<<2;
        }

#ifndef SHEEN_VC_DEBUG
#ifndef MEDIA_VOCVID_SUPPORT
        if(fillAudioData && (y*mb_width)>=fillAudMbScale)//read audio when image big than qcif
        {
            fillAudioData();
            fillAudMbScale+=MBScale4AudRed;
        }
#endif
#endif
    }
}

#if 0//move to xvid.c for overlay

/* perform post processing if necessary, and output the image */
static void decoder_output(DECODER * dec, IMAGE * img,xvid_dec_frame_t * frame, xvid_dec_stats_t * stats,
                           int coding_type, zoomPar *pZoom)
{
//  frame->general = 1;
    image_output(img, dec->width, dec->height,
                 dec->edged_width, (uint8_t**)frame->output.plane, pZoom);

    if (stats)
    {
        stats->type = coding2type(coding_type);
        stats->data.vop.time_base = (int)dec->time_base;
        stats->data.vop.time_increment = 0; /* XXX: todo */
        stats->data.vop.qscale_stride = dec->mb_width;
    }
}

int
decoder_decode(DECODER * dec,
               xvid_dec_frame_t * frame, xvid_dec_stats_t * stats,zoomPar *pZoom)
{

    Bitstream bs;
    uint16_t rounding;
    uint16_t quant = 2;
    uint16_t fcode_forward;
    uint16_t intra_dc_threshold;
    int coding_type;
    int success, output, seen_something;

    if (XVID_VERSION_MAJOR(frame->version) != 1 || (stats && XVID_VERSION_MAJOR(stats->version) != 1))  /* v1.x.x */
        return XVID_ERR_VERSION;

    dec->low_delay_default = (frame->general & XVID_LOWDELAY);
    if ((frame->general & XVID_DISCONTINUITY))
        dec->frames = 0;

    dec->out_frm = (frame->output.csp == XVID_CSP_SLICE) ? &frame->output : NULL;

    if(frame->length<0)    /* decoder flush */
    {
        if (stats) stats->type = XVID_TYPE_NOTHING;
        return XVID_ERR_END;
    }

    BitstreamInit(&bs, frame->bitstream, frame->length);

    success = 0;
    output = 0;
    seen_something = 0;

repeat:

    coding_type = BitstreamReadHeaders(&bs, dec, &rounding,
                                       &quant, &fcode_forward, &intra_dc_threshold);

    if(coding_type == -8)//profile unfit
        return -1;
    if (coding_type == -1)   /* nothing */
    {
        if (success) goto done;
        if (stats) stats->type = XVID_TYPE_NOTHING;

        return BitstreamPos(&bs)>>3;
    }

    if (coding_type == -2 || coding_type == -3)   /* vol and/or resize */
    {

        if (coding_type == -3)
            decoder_resize(dec);

        if(stats)
        {
            stats->type = XVID_TYPE_VOL;
            stats->data.vol.general = 0;
            stats->data.vol.width = dec->width;
            stats->data.vol.height = dec->height;
            stats->data.vol.par = dec->aspect_ratio;
            stats->data.vol.par_width = dec->par_width;
            stats->data.vol.par_height = dec->par_height;

            return BitstreamPos(&bs)>>3; /* number of bytes consumed */
        }
        goto repeat;
    }

    if(dec->frames == 0 && coding_type != I_VOP)
    {
        /* 1st frame is not an i-vop */
        goto repeat;
    }
    dec->p_fmv.x = dec->p_fmv.y = 0;  /* init pred vector to 0 */
    switch(coding_type)
    {
        case I_VOP :
            decoder_iframe(dec, &bs, quant, intra_dc_threshold);
//    frame_number ++;//test by longyl
            break;
        case P_VOP :
            decoder_pframe(dec, &bs, rounding, quant,
                           fcode_forward, intra_dc_threshold);//, NULL);
//    frame_number ++;//test by longyl
            break;
        case N_VOP :
            /* XXX: not_coded vops are not used for forward prediction */
            /* we should not swap(last_mbs,mbs) */
            image_copy(&dec->cur, &dec->refn[0], dec->edged_width, dec->height>>mpeg4_lowres);
            break;
    }
#if 0
    if (dec->frames > 0)   /* is the reference frame valid? */
    {
        /* output the reference frame */
        decoder_output(dec, &dec->refn[0], frame, stats, dec->last_coding_type,  pZoom);
        output = 1;
    }
#else// output current decode frame
    decoder_output(dec, &dec->cur, frame, stats, coding_type,  pZoom);
#endif
    image_swap(&dec->cur, &dec->refn[0]);
    dec->is_edged[0] = 0;
    dec->last_coding_type = coding_type;

//  diag_printf("########     decoder_decode have finished a frame ##########!\n");

    dec->frames++;
    seen_something = 1;

done :

    return (BitstreamPos(&bs)+7)>>3; /* number of bytes consumed */
}

#endif

#ifndef VOC_DO
static
int gmc_sanitize(int value, int quarterpel, int fcode)
{
    int length = 1 << (fcode+4);

#if 0
    if (quarterpel) value *= 2;
#endif

    if (value < -length)
        return -length;
    else if (value >= length)
        return length-1;
    else return value;
}

void
decoder_mbgmc(DECODER * dec,
              VECTOR *pMVBuffPred[3],
              uint8_t **block,
              const uint32_t x_pos,
              const uint32_t y_pos,
              const uint32_t fcode,
              const uint32_t cbp,
              Bitstream * bs,
              const uint32_t rounding)
{
    const uint32_t stride = dec->edged_width;
    const uint32_t stride2 = stride / 2;

    uint8_t *const pY_Cur=dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
    uint8_t *const pU_Cur=dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
    uint8_t *const pV_Cur=dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

    NEW_GMC_DATA * gmc_data = &dec->new_gmc_data;

    dec->mvs[0] = dec->mvs[1] = dec->mvs[2] = dec->mvs[3] = dec->amv;
    pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
                        = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];

    /* this is where the calculations are done */

    gmc_data->predict_16x16(gmc_data,
                            pY_Cur, dec->refn[0].y,
                            stride, stride, x_pos, y_pos, rounding);

    gmc_data->predict_8x8(gmc_data,
                          pU_Cur, dec->refn[0].u,
                          pV_Cur, dec->refn[0].v,
                          stride2, stride2, x_pos, y_pos, rounding);

    gmc_data->get_average_mv(gmc_data, &dec->amv, x_pos, y_pos, dec->quarterpel);

    dec->amv.x = gmc_sanitize(dec->amv.x, dec->quarterpel, fcode);
    dec->amv.y = gmc_sanitize(dec->amv.y, dec->quarterpel, fcode);

    dec->mvs[0] = dec->mvs[1] = dec->mvs[2] = dec->mvs[3] = dec->amv;
    pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
                        = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];
    {
        uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t y_stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

        uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t uv_stride_tmp = (dec->edged_width >> 3);

        uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;
        uint8_t i;
        for(i = 8; i > 0; i--)
        {
            y_ref_addr_tmp0[0]  = y_dst_addr_tmp[0];
            y_ref_addr_tmp0[1]  = y_dst_addr_tmp[1];
            y_ref_addr_tmp0[16] = y_dst_addr_tmp[2];
            y_ref_addr_tmp0[17] = y_dst_addr_tmp[3];

            y_ref_addr_tmp0[32] = dst_addr_stride[0];
            y_ref_addr_tmp0[33] = dst_addr_stride[1];
            y_ref_addr_tmp0[48] = dst_addr_stride[2];
            y_ref_addr_tmp0[49] = dst_addr_stride[3];

            y_dst_addr_tmp += y_stride_tmp;
            dst_addr_stride += y_stride_tmp;
            y_ref_addr_tmp0 += 2;

            u_ref_addr_tmp0[0] = u_dst_addr_tmp[0];
            u_ref_addr_tmp0[1] = u_dst_addr_tmp[1];

            u_dst_addr_tmp += uv_stride_tmp;
            u_ref_addr_tmp0 += 2;

            v_ref_addr_tmp0[0] = v_dst_addr_tmp[0];
            v_ref_addr_tmp0[1] = v_dst_addr_tmp[1];

            v_dst_addr_tmp += uv_stride_tmp;
            v_ref_addr_tmp0 += 2;

        }
    }

    if (cbp)
        decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);

    {
        uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t y_stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

        uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t uv_stride_tmp = (dec->edged_width >> 3);

        uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;
        uint8_t i;
        for(i = 8; i > 0; i--)
        {
            y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
            y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
            y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
            y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

            dst_addr_stride[0] = y_ref_addr_tmp0[32];
            dst_addr_stride[1] = y_ref_addr_tmp0[33];
            dst_addr_stride[2] = y_ref_addr_tmp0[48];
            dst_addr_stride[3] = y_ref_addr_tmp0[49];

            y_dst_addr_tmp += y_stride_tmp;
            dst_addr_stride += y_stride_tmp;
            y_ref_addr_tmp0 += 2;

            u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
            u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

            u_dst_addr_tmp += uv_stride_tmp;
            u_ref_addr_tmp0 += 2;

            v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
            v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

            v_dst_addr_tmp += uv_stride_tmp;
            v_ref_addr_tmp0 += 2;
        }
    }

}

void dec_mb_inter1v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding)
{
    VECTOR pmv;
    int16_t coord_x,coord_y;
    int32_t y_add_temp,uv_add_temp;

    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
    int y_dx,y_dy,uv_dx, uv_dy,i;

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
    dec_mv(fcode,&dec->mvs[0],&pmv,bs);
    pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
                        = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];
    if(mpeg4_lowres ==1)
    {
        pY_Cur = dec->y_row_start + (x_pos<<3);
        pU_Cur = dec->u_row_start + (x_pos<<2);
        pV_Cur = dec->v_row_start + (x_pos<<2);

        MPV_mpeg4_motion_lowres(dec,0,pY_Cur ,pU_Cur ,pV_Cur,x_pos,y_pos,0,dec->refn);
        if (cbp)
        {
            decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur);
        }

    }
    else
    {
        coord_x = x_pos << 4;
        coord_y = y_pos << 4;

        y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
        uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
        pY_Cur = dec->y_row_start + coord_x;
        pU_Cur = dec->u_row_start + (x_pos << 3);
        pV_Cur = dec->v_row_start + (x_pos << 3);
        {
            int16_t shift = 1 + dec->quarterpel;
            int16_t coord_tmp = (dec->width-coord_x)<<shift;
            int mv_dtmp = dec->mvs[0].x + ((EDGE_SIZE + coord_x)<<shift);
            y_dx = dec->mvs[0].x + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y_dx - coord_tmp;
            y_dx = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);

            mv_dtmp = dec->mvs[0].y + ((EDGE_SIZE + coord_y)<<shift);
            y_dy = dec->mvs[0].y + ((mv_dtmp>>31)&(-mv_dtmp));
            coord_tmp = (dec->height-coord_y)<<shift;
            mv_dtmp = y_dy - coord_tmp;
            y_dy = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);
        }
        if(dec->quarterpel)
        {
            uv_dx = y_dx / 2;
            uv_dy = y_dy / 2;
        }
        else
        {
            uv_dx = y_dx;
            uv_dy = y_dy;
        }
        uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
        uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];
        if (dec->quarterpel)
        {
            interpolate16x16_quarterpel(block[0], dec->refn[0].y + y_add_temp,
                                        y_dx, y_dy, dec->stride[0], rounding);
        }
        else
        {
            interpolate16x16_switch(block[0], dec->refn[0].y + y_add_temp,
                                    y_dx, y_dy, dec->stride[0], rounding);
        }
        interpolate8x8_switch(block[1],dec->refn[0].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], rounding);
        interpolate8x8_switch(block[2],dec->refn[0].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], rounding);

        if (cbp)
        {
            decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
        }
#if 0
        //y
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
            uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 2);
            uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];
                dst_addr_tmp[2] = ref_addr_tmp0[16];
                dst_addr_tmp[3] = ref_addr_tmp0[17];

                dst_addr_stride[0] = ref_addr_tmp0[32];
                dst_addr_stride[1] = ref_addr_tmp0[33];
                dst_addr_stride[2] = ref_addr_tmp0[48];
                dst_addr_stride[3] = ref_addr_tmp0[49];

                dst_addr_tmp += stride_tmp;
                dst_addr_stride += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
        //u
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
            uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 3);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];

                dst_addr_tmp += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
        //v
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
            uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 3);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];

                dst_addr_tmp += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
#else
        {
            uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
            uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
            uint32_t y_stride_tmp = (dec->edged_width >> 2);
            uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

            uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
            uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
            uint32_t uv_stride_tmp = (dec->edged_width >> 3);

            uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
            uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

            for(i = 8; i > 0; i--)
            {
                y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
                y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
                y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
                y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

                dst_addr_stride[0] = y_ref_addr_tmp0[32];
                dst_addr_stride[1] = y_ref_addr_tmp0[33];
                dst_addr_stride[2] = y_ref_addr_tmp0[48];
                dst_addr_stride[3] = y_ref_addr_tmp0[49];

                y_dst_addr_tmp += y_stride_tmp;
                dst_addr_stride += y_stride_tmp;
                y_ref_addr_tmp0 += 2;

                u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
                u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

                u_dst_addr_tmp += uv_stride_tmp;
                u_ref_addr_tmp0 += 2;

                v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
                v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

                v_dst_addr_tmp += uv_stride_tmp;
                v_ref_addr_tmp0 += 2;
            }
        }
#endif
    }
//  macroblock_number ++;//test by longyl
}

void dec_mb_inter4v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding)
{
    VECTOR pmv;
    int32_t coord_x,coord_y;
    int32_t y_add_temp,uv_add_temp;

    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
    uint32_t next_pos;
    int uv_dx, uv_dy;
    int y1_dx,y2_dx,y3_dx,y4_dx,y1_dy,y2_dy,y3_dy,y4_dy,i;

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
    dec_mv(fcode,&dec->mvs[0],&pmv,bs);
    pMVBuffPred[0][0] = pMVBuffPred[1][0] = dec->mvs[0];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,1);
    dec_mv(fcode,&dec->mvs[1],&pmv,bs);
    pMVBuffPred[0][1] = pMVBuffPred[1][1] = dec->mvs[1];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,2);
    dec_mv(fcode,&dec->mvs[2],&pmv,bs);
    pMVBuffPred[0][2] = pMVBuffPred[1][2] = dec->mvs[2];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,3);
    dec_mv(fcode,&dec->mvs[3],&pmv,bs);
    pMVBuffPred[0][3] = pMVBuffPred[1][3] = dec->mvs[3];

    if(mpeg4_lowres ==1)
    {

        pY_Cur = dec->y_row_start + (x_pos<<3);
        pU_Cur = dec->u_row_start + (x_pos<<2);
        pV_Cur = dec->v_row_start + (x_pos<<2);


        MPV_mpeg4_motion_lowres(dec,1,pY_Cur ,pU_Cur ,pV_Cur,x_pos,y_pos,0,dec->refn);

        if (cbp)
        {
            decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur);
        }


    }
    else
    {
        next_pos = dec->edged_width << 3;
        coord_x = x_pos << 4;
        coord_y = y_pos << 4;

        /*y_add_temp = coord_y * dec->edged_width + coord_x;
        uv_add_temp = (y_add_temp + coord_x) >> 2;*/
        y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
        uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
        pY_Cur = dec->y_row_start + coord_x;
        pU_Cur = dec->u_row_start + (x_pos << 3);
        pV_Cur = dec->v_row_start + (x_pos << 3);
        {
            int16_t shift = 1 + dec->quarterpel;
            int16_t coordx_tmp = (dec->width-coord_x)<<shift;
            int16_t coordy_tmp = (dec->height-coord_y)<<shift;
            int32_t coordx_tmp2 = (EDGE_SIZE + coord_x)<<shift;
            int32_t coordy_tmp2 = (EDGE_SIZE + coord_y)<<shift;
            int mv_dtmp = dec->mvs[0].x + coordx_tmp2;
            y1_dx = dec->mvs[0].x + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y1_dx - coordx_tmp;
            y1_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
            mv_dtmp = dec->mvs[0].y + coordy_tmp2;
            y1_dy = dec->mvs[0].y + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y1_dy - coordy_tmp;
            y1_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

            mv_dtmp = dec->mvs[1].x + coordx_tmp2;
            y2_dx = dec->mvs[1].x + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y2_dx - coordx_tmp;
            y2_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
            mv_dtmp = dec->mvs[1].y + coordy_tmp2;
            y2_dy = dec->mvs[1].y + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y2_dy - coordy_tmp;
            y2_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

            mv_dtmp = dec->mvs[2].x + coordx_tmp2;
            y3_dx = dec->mvs[2].x + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y3_dx - coordx_tmp;
            y3_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
            mv_dtmp = dec->mvs[2].y + coordy_tmp2;
            y3_dy = dec->mvs[2].y + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y3_dy - coordy_tmp;
            y3_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

            mv_dtmp = dec->mvs[3].x + coordx_tmp2;
            y4_dx = dec->mvs[3].x + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y4_dx - coordx_tmp;
            y4_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
            mv_dtmp = dec->mvs[3].y + coordy_tmp2;
            y4_dy = dec->mvs[3].y + ((mv_dtmp>>31)&(-mv_dtmp));
            mv_dtmp = y4_dy - coordy_tmp;
            y4_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);
        }
        if(dec->quarterpel)
        {
            uv_dx = y1_dx/2 + y2_dx/2 + y3_dx/2 + y4_dx/2;
            uv_dy = y1_dy/2 + y2_dy/2 + y3_dy/2 + y4_dy/2;
        }
        else
        {
            uv_dx = y1_dx + y2_dx + y3_dx + y4_dx;
            uv_dy = y1_dy + y2_dy + y3_dy + y4_dy;
        }
        uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
        uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];
        if(dec->quarterpel)
        {
            interpolate8x8_quarterpel(block[0], dec->refn[0].y + y_add_temp,
                                      y1_dx, y1_dy, dec->stride[0], rounding);
            interpolate8x8_quarterpel(block[0]+64, dec->refn[0].y + y_add_temp+8,
                                      y2_dx, y2_dy, dec->stride[1], rounding);
            interpolate8x8_quarterpel(block[0]+128, dec->refn[0].y + y_add_temp+next_pos,
                                      y3_dx, y3_dy, dec->stride[2], rounding);
            interpolate8x8_quarterpel(block[0]+192, dec->refn[0].y + y_add_temp+next_pos+8,
                                      y4_dx, y4_dy, dec->stride[3], rounding);
        }
        else
        {
            interpolate8x8_switch(block[0], dec->refn[0].y + y_add_temp,
                                  y1_dx, y1_dy, dec->stride[0], rounding);
            interpolate8x8_switch(block[0]+64, dec->refn[0].y + y_add_temp+8,
                                  y2_dx, y2_dy, dec->stride[1], rounding);
            interpolate8x8_switch(block[0]+128, dec->refn[0].y + y_add_temp+next_pos,
                                  y3_dx, y3_dy, dec->stride[2], rounding);
            interpolate8x8_switch(block[0]+192, dec->refn[0].y + y_add_temp+next_pos+8,
                                  y4_dx, y4_dy, dec->stride[3], rounding);
        }
        interpolate8x8_switch(block[1],dec->refn[0].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], rounding);
        interpolate8x8_switch(block[2],dec->refn[0].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], rounding);
        if (cbp)
        {
            decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
        }
#if 0
        //y
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
            uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 2);
            uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];
                dst_addr_tmp[2] = ref_addr_tmp0[16];
                dst_addr_tmp[3] = ref_addr_tmp0[17];

                dst_addr_stride[0] = ref_addr_tmp0[32];
                dst_addr_stride[1] = ref_addr_tmp0[33];
                dst_addr_stride[2] = ref_addr_tmp0[48];
                dst_addr_stride[3] = ref_addr_tmp0[49];

                dst_addr_tmp += stride_tmp;
                dst_addr_stride += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
        //u
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
            uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 3);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];

                dst_addr_tmp += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
        //v
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
            uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
            uint32_t stride_tmp = (dec->edged_width >> 3);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];

                dst_addr_tmp += stride_tmp;
                ref_addr_tmp0 += 2;
            }
        }
#else
        {
            uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
            uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
            uint32_t y_stride_tmp = (dec->edged_width >> 2);
            uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

            uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
            uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
            uint32_t uv_stride_tmp = (dec->edged_width >> 3);

            uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
            uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

            for(i = 8; i > 0; i--)
            {
                y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
                y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
                y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
                y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

                dst_addr_stride[0] = y_ref_addr_tmp0[32];
                dst_addr_stride[1] = y_ref_addr_tmp0[33];
                dst_addr_stride[2] = y_ref_addr_tmp0[48];
                dst_addr_stride[3] = y_ref_addr_tmp0[49];

                y_dst_addr_tmp += y_stride_tmp;
                dst_addr_stride += y_stride_tmp;
                y_ref_addr_tmp0 += 2;

                u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
                u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

                u_dst_addr_tmp += uv_stride_tmp;
                u_ref_addr_tmp0 += 2;

                v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
                v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

                v_dst_addr_tmp += uv_stride_tmp;
                v_ref_addr_tmp0 += 2;
            }
        }
    }
#endif
//  macroblock_number ++;//test by longyl
    }

    /* decode an B-frame direct & interpolate macroblock */
#if 0
    static void
    decoder_bf_interpolate_mbinter(DECODER * dec,
                                   IMAGE forward,
                                   IMAGE backward,
                                   MACROBLOCK * pMB,
                                   uint8_t **block,
                                   const uint32_t x_pos,
                                   const uint32_t y_pos,
                                   Bitstream * bs,
                                   const int direct)
    {
        uint32_t stride = dec->edged_width;
        uint32_t stride2 = stride / 2;
        int uv_dx, uv_dy;
        int b_uv_dx, b_uv_dy;
        uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
        const uint32_t cbp ;//= pMB->cbp;

        pY_Cur = dec->cur.y + (y_pos << 4) * stride + (x_pos << 4);
        pU_Cur = dec->cur.u + (y_pos << 3) * stride2 + (x_pos << 3);
        pV_Cur = dec->cur.v + (y_pos << 3) * stride2 + (x_pos << 3);

        //validate_vector(pMB->mvs, x_pos, y_pos, dec);
        //validate_vector(pMB->b_mvs, x_pos, y_pos, dec);

        if (!direct)
        {
            uv_dx = dec->mvs[0].x;
            uv_dy = dec->mvs[0].y;
            b_uv_dx = dec->bmvs[0].x;
            b_uv_dy = dec->bmvs[0].y;
#if 0

            if (dec->quarterpel)
            {
                if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING)
                {
                    uv_dx = (uv_dx>>1) | (uv_dx&1);
                    uv_dy = (uv_dy>>1) | (uv_dy&1);
                    b_uv_dx = (b_uv_dx>>1) | (b_uv_dx&1);
                    b_uv_dy = (b_uv_dy>>1) | (b_uv_dy&1);
                }
                else
                {
                    uv_dx /= 2;
                    uv_dy /= 2;
                    b_uv_dx /= 2;
                    b_uv_dy /= 2;
                }
            }
#endif

            uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
            uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];
            b_uv_dx = (b_uv_dx >> 1) + roundtab_79[b_uv_dx & 0x3];
            b_uv_dy = (b_uv_dy >> 1) + roundtab_79[b_uv_dy & 0x3];

        }
        else
        {
#if 0
            if (dec->quarterpel)   /* for qpel the /2 shall be done before summation. We've done it right in the encoder in the past. */
            {
                /* TODO: figure out if we ever did it wrong on the encoder side. If yes, add some workaround */
                if (dec->bs_version <= BS_VERSION_BUGGY_CHROMA_ROUNDING)
                {
                    int z;
                    uv_dx = 0; uv_dy = 0;
                    b_uv_dx = 0; b_uv_dy = 0;
                    for (z = 0; z < 4; z++)
                    {
                        //    uv_dx += ((pMB->mvs[z].x>>1) | (pMB->mvs[z].x&1));
                        //    uv_dy += ((pMB->mvs[z].y>>1) | (pMB->mvs[z].y&1));
                        //    b_uv_dx += ((pMB->b_mvs[z].x>>1) | (pMB->b_mvs[z].x&1));
                        //        b_uv_dy += ((pMB->b_mvs[z].y>>1) | (pMB->b_mvs[z].y&1));
                    }
                }
                else
                {
                    //      uv_dx = (pMB->mvs[0].x / 2) + (pMB->mvs[1].x / 2) + (pMB->mvs[2].x / 2) + (pMB->mvs[3].x / 2);
                    //      uv_dy = (pMB->mvs[0].y / 2) + (pMB->mvs[1].y / 2) + (pMB->mvs[2].y / 2) + (pMB->mvs[3].y / 2);
                    //      b_uv_dx = (pMB->b_mvs[0].x / 2) + (pMB->b_mvs[1].x / 2) + (pMB->b_mvs[2].x / 2) + (pMB->b_mvs[3].x / 2);
                    //      b_uv_dy = (pMB->b_mvs[0].y / 2) + (pMB->b_mvs[1].y / 2) + (pMB->b_mvs[2].y / 2) + (pMB->b_mvs[3].y / 2);
                }
            }
            else
#endif
            {
                uv_dx = dec->mvs[0].x + dec->mvs[1].x + dec->mvs[2].x + dec->mvs[3].x;
                uv_dy = dec->mvs[0].y + dec->mvs[1].y + dec->mvs[2].y + dec->mvs[3].y;
                b_uv_dx = dec->bmvs[0].x + dec->bmvs[1].x + dec->bmvs[2].x + dec->bmvs[3].x;
                b_uv_dy = dec->bmvs[0].y + dec->bmvs[1].y + dec->bmvs[2].y + dec->bmvs[3].y;
            }

            uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
            uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];
            b_uv_dx = (b_uv_dx >> 3) + roundtab_76[b_uv_dx & 0xf];
            b_uv_dy = (b_uv_dy >> 3) + roundtab_76[b_uv_dy & 0xf];
        }
#if 0
        if(dec->quarterpel)
        {
            if(!direct)
            {
                //  interpolate16x16_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
                //                dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                //                pMB->mvs[0].x, pMB->mvs[0].y, stride, 0);
            }
            else
            {
                //  interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
                //               dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                //                pMB->mvs[0].x, pMB->mvs[0].y, stride, 0);
                //  interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
                //               dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
                //                pMB->mvs[1].x, pMB->mvs[1].y, stride, 0);
                // interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
                //                dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
                //                pMB->mvs[2].x, pMB->mvs[2].y, stride, 0);
                // interpolate8x8_quarterpel(dec->cur.y, forward.y, dec->qtmp.y, dec->qtmp.y + 64,
                //                dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
                //                pMB->mvs[3].x, pMB->mvs[3].y, stride, 0);
            }
        }
        else
#endif
        {
            interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos, 16 * y_pos,
                                  dec->mvs[0].x, dec->mvs[0].y, stride, 0);
            interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos + 8, 16 * y_pos,
                                  dec->mvs[1].x, dec->mvs[1].y, stride, 0);
            interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos, 16 * y_pos + 8,
                                  dec->mvs[2].x, dec->mvs[2].y, stride, 0);
            interpolate8x8_switch(dec->cur.y, forward.y, 16 * x_pos + 8, 16 * y_pos + 8,
                                  dec->mvs[3].x, dec->mvs[3].y, stride, 0);
        }

        interpolate8x8_switch(dec->cur.u, forward.u, 8 * x_pos, 8 * y_pos, uv_dx,
                              uv_dy, stride2, 0);
        interpolate8x8_switch(dec->cur.v, forward.v, 8 * x_pos, 8 * y_pos, uv_dx,
                              uv_dy, stride2, 0);

#if 0

        if(dec->quarterpel)
        {
            if(!direct)
            {
                interpolate16x16_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
                                                dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                                                pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0);
            }
            else
            {
                interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
                                              dec->qtmp.y + 128, 16*x_pos, 16*y_pos,
                                              pMB->b_mvs[0].x, pMB->b_mvs[0].y, stride, 0);
                interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
                                              dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos,
                                              pMB->b_mvs[1].x, pMB->b_mvs[1].y, stride, 0);
                interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
                                              dec->qtmp.y + 128, 16*x_pos, 16*y_pos + 8,
                                              pMB->b_mvs[2].x, pMB->b_mvs[2].y, stride, 0);
                interpolate8x8_add_quarterpel(dec->cur.y, backward.y, dec->qtmp.y, dec->qtmp.y + 64,
                                              dec->qtmp.y + 128, 16*x_pos + 8, 16*y_pos + 8,
                                              pMB->b_mvs[3].x, pMB->b_mvs[3].y, stride, 0);
            }
        }
        else

#endif

        {
            interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos, 16 * y_pos,
                                      dec->bmvs[0].x, dec->bmvs[0].y, stride, 0);
            interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos + 8,
                                      16 * y_pos, dec->bmvs[1].x, dec->bmvs[1].y, stride, 0);
            interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos,
                                      16 * y_pos + 8, dec->bmvs[2].x, dec->bmvs[2].y, stride, 0);
            interpolate8x8_add_switch(dec->cur.y, backward.y, 16 * x_pos + 8,
                                      16 * y_pos + 8, dec->bmvs[3].x, dec->bmvs[3].y, stride, 0);
        }

        interpolate8x8_add_switch(dec->cur.u, backward.u, 8 * x_pos, 8 * y_pos,
                                  b_uv_dx, b_uv_dy, stride2, 0);
        interpolate8x8_add_switch(dec->cur.v, backward.v, 8 * x_pos, 8 * y_pos,
                                  b_uv_dx, b_uv_dy, stride2, 0);

        if (cbp)
            decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur, pMB);
    }
#endif
    void decoder_bf_interpolate_mbinter(DECODER *dec,uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                                        Bitstream *bs, int direct)
    {
        VECTOR pmv;
        int16_t coord_x,coord_y;
        int32_t y_add_temp,uv_add_temp;
        uint32_t next_pos;
        uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
        int y_dx[4],y_dy[4],uv_dx, uv_dy,y_bdx[4],y_bdy[4],uv_bdx, uv_bdy,i;

        //pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
        //dec_mv(fcode,&dec->mvs[0],&pmv,bs);
        //pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
        //  = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];
#if 0
        if(mpeg4_lowres ==1)
        {
            pY_Cur = dec->y_row_start + (x_pos<<3);
            pU_Cur = dec->u_row_start + (x_pos<<2);
            pV_Cur = dec->v_row_start + (x_pos<<2);

            MPV_mpeg4_motion_lowres(dec,0,pY_Cur ,pU_Cur ,pV_Cur,x_pos,y_pos,0,dec->refn);
            if (cbp)
            {
                decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur);
            }

        }
        else
#endif
        {
            coord_x = x_pos << 4;
            coord_y = y_pos << 4;
            next_pos = dec->edged_width << 3;

            y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
            uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
            pY_Cur = dec->y_row_start + coord_x;
            pU_Cur = dec->u_row_start + (x_pos << 3);
            pV_Cur = dec->v_row_start + (x_pos << 3);
            {
                int32 mv_dtmp;
                int16_t shift = 1 + dec->quarterpel;
                int16_t coordx_tmp = (dec->width-coord_x)<<shift;
                int16_t coordy_tmp = (dec->height-coord_y)<<shift;
                int32_t coordx_tmp2 = (EDGE_SIZE + coord_x)<<shift;
                int32_t coordy_tmp2 = (EDGE_SIZE + coord_y)<<shift;
                for(i=0; i<4; i++)
                {
                    mv_dtmp = dec->mvs[i].x + coordx_tmp2;
                    y_dx[i] = dec->mvs[i].x + ((mv_dtmp>>31)&(-mv_dtmp));
                    mv_dtmp = y_dx[i] - coordx_tmp;
                    y_dx[i] = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);

                    mv_dtmp = dec->bmvs[i].x + coordx_tmp2;
                    y_bdx[i] = dec->bmvs[i].x + ((mv_dtmp>>31)&(-mv_dtmp));
                    mv_dtmp = y_bdx[i] - coordx_tmp;
                    y_bdx[i] = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);

                    mv_dtmp = dec->mvs[i].y + coordy_tmp2;
                    y_dy[i] = dec->mvs[i].y + ((mv_dtmp>>31)&(-mv_dtmp));
                    mv_dtmp = y_dy[i] - coordy_tmp;
                    y_dy[i] = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

                    mv_dtmp = dec->bmvs[i].y + coordy_tmp2;
                    y_bdy[i] = dec->bmvs[i].y + ((mv_dtmp>>31)&(-mv_dtmp));
                    mv_dtmp = y_bdy[i] - coordy_tmp;
                    y_bdy[i] = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

                }
                if(!direct)
                {
                    //uv_dx = (y_dx[0] + y_dx[1] +y_dx[2] +y_dx[3])>>3;
                    //uv_dy = (y_dy[0] + y_dy[1] +y_dy[2] +y_dy[3])>>3;
                    //uv_bdx = (y_bdx[0] + y_bdx[1] +y_bdx[2] +y_bdx[3])>>3;
                    //uv_bdy = (y_bdy[0] + y_bdy[1] +y_bdy[2] +y_bdy[3])>>3;
                    uv_dx = y_dx[0];
                    uv_dy = y_dy[0];
                    uv_bdx = y_bdx[0];
                    uv_bdy = y_bdy[0];
                    if(dec->quarterpel)
                    {
                        //uv_dx = (y_dx[0] + y_dx[1] +y_dx[2] +y_dx[3])>>3;
                        //uv_dy = (y_dy[0] + y_dy[1] +y_dy[2] +y_dy[3])>>3;
                        //uv_bdx = (y_bdx[0] + y_bdx[1] +y_bdx[2] +y_bdx[3])>>3;
                        //uv_bdy = (y_bdy[0] + y_bdy[1] +y_bdy[2] +y_bdy[3])>>3;
                        uv_dx /= 2;
                        uv_dy /= 2;
                        uv_bdx /= 2;
                        uv_bdy /= 2;
                    }
                    uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
                    uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];
                    uv_bdx = (uv_bdx >> 1) + roundtab_79[uv_bdx & 0x3];
                    uv_bdy = (uv_bdy >> 1) + roundtab_79[uv_bdy & 0x3];
                }
                else
                {

                    if(dec->quarterpel)
                    {
                        uv_dx = y_dx[0]/2 + y_dx[1]/2 + y_dx[2]/2 +y_dx[3]/2;
                        uv_dy = y_dy[0]/2 + y_dy[1]/2 +y_dy[2]/2 +y_dy[3]/2;
                        uv_bdx = y_bdx[0]/2 + y_bdx[1]/2 +y_bdx[2]/2 +y_bdx[3]/2;
                        uv_bdy = y_bdy[0]/2 + y_bdy[1]/2 +y_bdy[2]/2 +y_bdy[3]/2;
                    }
                    else
                    {
                        uv_dx = (y_dx[0] + y_dx[1] +y_dx[2] +y_dx[3]);
                        uv_dy = (y_dy[0] + y_dy[1] +y_dy[2] +y_dy[3]);
                        uv_bdx = (y_bdx[0] + y_bdx[1] +y_bdx[2] +y_bdx[3]);
                        uv_bdy = (y_bdy[0] + y_bdy[1] +y_bdy[2] +y_bdy[3]);
                    }
                    uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
                    uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];
                    uv_bdx = (uv_bdx >> 3) + roundtab_76[uv_bdx & 0xf];
                    uv_bdy = (uv_bdy >> 3) + roundtab_76[uv_bdy & 0xf];

                }
            }


            //interpolate16x16_switch(block[0], dec->refn[1].y + y_add_temp,
            //              y_dx, y_dy, dec->stride[0], 0);
            if(dec->quarterpel)
            {
                interpolate8x8_quarterpel(block[0], dec->refn[1].y + y_add_temp,
                                          y_dx[0], y_dy[0], dec->stride[0], 0);
                interpolate8x8_quarterpel(block[0]+64, dec->refn[1].y + y_add_temp+8,
                                          y_dx[1], y_dy[1], dec->stride[1], 0);
                interpolate8x8_quarterpel(block[0]+128, dec->refn[1].y + y_add_temp+next_pos,
                                          y_dx[2], y_dy[2], dec->stride[2], 0);
                interpolate8x8_quarterpel(block[0]+192, dec->refn[1].y + y_add_temp+next_pos+8,
                                          y_dx[3], y_dy[3], dec->stride[3], 0);
            }
            else
            {
                interpolate8x8_switch(block[0], dec->refn[1].y + y_add_temp,
                                      y_dx[0], y_dy[0], dec->stride[0], 0);
                interpolate8x8_switch(block[0]+64, dec->refn[1].y + y_add_temp+8,
                                      y_dx[1], y_dy[1], dec->stride[1], 0);
                interpolate8x8_switch(block[0]+128, dec->refn[1].y + y_add_temp+next_pos,
                                      y_dx[2], y_dy[2], dec->stride[2], 0);
                interpolate8x8_switch(block[0]+192, dec->refn[1].y + y_add_temp+next_pos+8,
                                      y_dx[3], y_dy[3], dec->stride[3], 0);
            }
            interpolate8x8_switch(block[1],dec->refn[1].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], 0);
            interpolate8x8_switch(block[2],dec->refn[1].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], 0);

            if(dec->quarterpel)
            {
                interpolate8x8_add_quarterpel(block[0], dec->refn[0].y + y_add_temp,
                                              y_bdx[0], y_bdy[0], dec->stride[0], 0);
                interpolate8x8_add_quarterpel(block[0]+64, dec->refn[0].y + y_add_temp+8,
                                              y_bdx[1], y_bdy[1], dec->stride[1], 0);
                interpolate8x8_add_quarterpel(block[0]+128, dec->refn[0].y + y_add_temp+next_pos,
                                              y_bdx[2], y_bdy[2], dec->stride[2], 0);
                interpolate8x8_add_quarterpel(block[0]+192, dec->refn[0].y + y_add_temp+next_pos+8,
                                              y_bdx[3], y_bdy[3], dec->stride[3], 0);
            }
            else
            {

                interpolate8x8_add_switch(block[0], dec->refn[0].y + y_add_temp,
                                          y_bdx[0], y_bdy[0], dec->stride[0], 0);
                interpolate8x8_add_switch(block[0]+64, dec->refn[0].y + y_add_temp+8,
                                          y_bdx[1], y_bdy[1], dec->stride[1], 0);
                interpolate8x8_add_switch(block[0]+128, dec->refn[0].y + y_add_temp+next_pos,
                                          y_bdx[2], y_bdy[2], dec->stride[2], 0);
                interpolate8x8_add_switch(block[0]+192, dec->refn[0].y + y_add_temp+next_pos+8,
                                          y_bdx[3], y_bdy[3], dec->stride[3], 0);
            }
            interpolate8x8_add_switch(block[1], dec->refn[0].u + uv_add_temp,
                                      uv_bdx, uv_bdy, dec->stride[4], 0);
            interpolate8x8_add_switch(block[2], dec->refn[0].v + uv_add_temp,
                                      uv_bdx, uv_bdy, dec->stride[5], 0);


            if (cbp)
            {
                decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
            }
#if 0
            //y
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
                uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 2);
                uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];
                    dst_addr_tmp[2] = ref_addr_tmp0[16];
                    dst_addr_tmp[3] = ref_addr_tmp0[17];

                    dst_addr_stride[0] = ref_addr_tmp0[32];
                    dst_addr_stride[1] = ref_addr_tmp0[33];
                    dst_addr_stride[2] = ref_addr_tmp0[48];
                    dst_addr_stride[3] = ref_addr_tmp0[49];

                    dst_addr_tmp += stride_tmp;
                    dst_addr_stride += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
            //u
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
                uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 3);

                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];

                    dst_addr_tmp += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
            //v
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
                uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 3);

                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];

                    dst_addr_tmp += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
#else
            {
                uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
                uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
                uint32_t y_stride_tmp = (dec->edged_width >> 2);
                uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

                uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
                uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
                uint32_t uv_stride_tmp = (dec->edged_width >> 3);

                uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
                uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

                for(i = 8; i > 0; i--)
                {
                    y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
                    y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
                    y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
                    y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

                    dst_addr_stride[0] = y_ref_addr_tmp0[32];
                    dst_addr_stride[1] = y_ref_addr_tmp0[33];
                    dst_addr_stride[2] = y_ref_addr_tmp0[48];
                    dst_addr_stride[3] = y_ref_addr_tmp0[49];

                    y_dst_addr_tmp += y_stride_tmp;
                    dst_addr_stride += y_stride_tmp;
                    y_ref_addr_tmp0 += 2;

                    u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
                    u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

                    u_dst_addr_tmp += uv_stride_tmp;
                    u_ref_addr_tmp0 += 2;

                    v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
                    v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

                    v_dst_addr_tmp += uv_stride_tmp;
                    v_ref_addr_tmp0 += 2;
                }
            }
#endif
        }
//  macroblock_number ++;//test by longyl
    }

    void dec_bf_mb_inter1v(DECODER *dec,uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                           Bitstream *bs,int rounding,int ref)
    {
        VECTOR pmv;
        int16_t coord_x,coord_y;
        int32_t y_add_temp,uv_add_temp;

        uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
        int y_dx,y_dy,uv_dx, uv_dy,i;

        //pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
        //dec_mv(fcode,&dec->mvs[0],&pmv,bs);
        //pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
        //  = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];
        if(ref == 1)
        {
            pmv = dec->mvs[0];
        }
        else
        {
            pmv = dec->bmvs[0];
        }

        if(mpeg4_lowres ==1)
        {
            pY_Cur = dec->y_row_start + (x_pos<<3);
            pU_Cur = dec->u_row_start + (x_pos<<2);
            pV_Cur = dec->v_row_start + (x_pos<<2);

            MPV_mpeg4_motion_lowres(dec,0,pY_Cur ,pU_Cur ,pV_Cur,x_pos,y_pos,0,dec->refn);
            if (cbp)
            {
                decoder_mb_decode(dec, cbp, bs, pY_Cur, pU_Cur, pV_Cur);
            }

        }
        else
        {
            coord_x = x_pos << 4;
            coord_y = y_pos << 4;

            y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
            uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
            pY_Cur = dec->y_row_start + coord_x;
            pU_Cur = dec->u_row_start + (x_pos << 3);
            pV_Cur = dec->v_row_start + (x_pos << 3);
            {
                int16_t shift = 1 + dec->quarterpel;
                int16_t coord_tmp = (dec->width-coord_x)<<shift;
                int mv_dtmp = pmv.x + ((EDGE_SIZE + coord_x)<<shift);
                y_dx = pmv.x + ((mv_dtmp>>31)&(-mv_dtmp));
                mv_dtmp = y_dx - coord_tmp;
                y_dx = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);

                mv_dtmp = pmv.y + ((EDGE_SIZE + coord_y)<<shift);
                y_dy = pmv.y + ((mv_dtmp>>31)&(-mv_dtmp));
                coord_tmp = (dec->height-coord_y)<<shift;
                mv_dtmp = y_dy - coord_tmp;
                y_dy = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);
            }
            if(dec->quarterpel)
            {
                uv_dx = y_dx / 2;
                uv_dy = y_dy / 2;
            }
            else
            {
                uv_dx = y_dx;
                uv_dy = y_dy;
            }
            uv_dx = (uv_dx >> 1) + roundtab_79[uv_dx & 0x3];
            uv_dy = (uv_dy >> 1) + roundtab_79[uv_dy & 0x3];
            if (dec->quarterpel)
            {
                interpolate16x16_quarterpel(block[0], dec->refn[ref].y + y_add_temp,
                                            y_dx, y_dy, dec->stride[0], rounding);
            }
            else
            {
                interpolate16x16_switch(block[0], dec->refn[ref].y + y_add_temp,
                                        y_dx, y_dy, dec->stride[0], rounding);
            }
            interpolate8x8_switch(block[1],dec->refn[ref].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], rounding);
            interpolate8x8_switch(block[2],dec->refn[ref].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], rounding);

            if (cbp)
            {
                decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
            }
#if 0
            //y
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
                uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 2);
                uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];
                    dst_addr_tmp[2] = ref_addr_tmp0[16];
                    dst_addr_tmp[3] = ref_addr_tmp0[17];

                    dst_addr_stride[0] = ref_addr_tmp0[32];
                    dst_addr_stride[1] = ref_addr_tmp0[33];
                    dst_addr_stride[2] = ref_addr_tmp0[48];
                    dst_addr_stride[3] = ref_addr_tmp0[49];

                    dst_addr_tmp += stride_tmp;
                    dst_addr_stride += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
            //u
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
                uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 3);

                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];

                    dst_addr_tmp += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
            //v
            {
                uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
                uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
                uint32_t stride_tmp = (dec->edged_width >> 3);

                for(i = 8; i > 0; i--)
                {
                    dst_addr_tmp[0] = ref_addr_tmp0[0];
                    dst_addr_tmp[1] = ref_addr_tmp0[1];

                    dst_addr_tmp += stride_tmp;
                    ref_addr_tmp0 += 2;
                }
            }
#else
            {
                uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
                uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
                uint32_t y_stride_tmp = (dec->edged_width >> 2);
                uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

                uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
                uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
                uint32_t uv_stride_tmp = (dec->edged_width >> 3);

                uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
                uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

                for(i = 8; i > 0; i--)
                {
                    y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
                    y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
                    y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
                    y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

                    dst_addr_stride[0] = y_ref_addr_tmp0[32];
                    dst_addr_stride[1] = y_ref_addr_tmp0[33];
                    dst_addr_stride[2] = y_ref_addr_tmp0[48];
                    dst_addr_stride[3] = y_ref_addr_tmp0[49];

                    y_dst_addr_tmp += y_stride_tmp;
                    dst_addr_stride += y_stride_tmp;
                    y_ref_addr_tmp0 += 2;

                    u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
                    u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

                    u_dst_addr_tmp += uv_stride_tmp;
                    u_ref_addr_tmp0 += 2;

                    v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
                    v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

                    v_dst_addr_tmp += uv_stride_tmp;
                    v_ref_addr_tmp0 += 2;
                }
            }
#endif
        }
//  macroblock_number ++;//test by longyl
    }

#endif

#endif

