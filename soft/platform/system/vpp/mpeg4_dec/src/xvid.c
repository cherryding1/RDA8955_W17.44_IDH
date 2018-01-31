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





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>

#include "xvid.h"
#include "decoder.h"
//#include "encoder.h"
//#include "bitstream/cbp.h"

//#ifdef NOACC
#include "idct.h"
#include "quant.h"
//#endif
//#include "dct/fdct.h"
//#include "colorspace.h"
#include "interpolate8x8.h"
#include "mem_transfer.h"
//#include "utils/mbfunctions.h"

//#include "motion/motion.h"
//#include "motion/sad.h"
//#include "utils/emms.h"
//#include "utils/timer.h"
#include "mbcoding.h"
//#include "image/qpel.h"
//#include "postprocessing.h"
#include "mem_align.h"

#if defined(_DEBUG)
unsigned int xvid_debug = 0; /* xvid debug mask */
#endif

/* detect cpu flags  */
#if 0
static unsigned int
detect_cpu_flags(void)
{
    /* enable native assembly optimizations by default */
    unsigned int cpu_flags = XVID_CPU_ASM;

    return cpu_flags;
}
#endif

//for mbcoding.c below
#define LEVELOFFSET 32
void init_vlc_tables(void);

REVERSE_EVENT DCT3D[2][4096];
VLC coeff_VLC[2][2][64][64];

/*****************************************************************************
 * VLC tables and other constant arrays
 ****************************************************************************/

VLC_TABLE const coeff_tab[2][102] =
{
    /* intra = 0 */
    {
        {{ 2,  2}, {0, 0, 1}},
        {{15,  4}, {0, 0, 2}},
        {{21,  6}, {0, 0, 3}},
        {{23,  7}, {0, 0, 4}},
        {{31,  8}, {0, 0, 5}},
        {{37,  9}, {0, 0, 6}},
        {{36,  9}, {0, 0, 7}},
        {{33, 10}, {0, 0, 8}},
        {{32, 10}, {0, 0, 9}},
        {{ 7, 11}, {0, 0, 10}},
        {{ 6, 11}, {0, 0, 11}},
        {{32, 11}, {0, 0, 12}},
        {{ 6,  3}, {0, 1, 1}},
        {{20,  6}, {0, 1, 2}},
        {{30,  8}, {0, 1, 3}},
        {{15, 10}, {0, 1, 4}},
        {{33, 11}, {0, 1, 5}},
        {{80, 12}, {0, 1, 6}},
        {{14,  4}, {0, 2, 1}},
        {{29,  8}, {0, 2, 2}},
        {{14, 10}, {0, 2, 3}},
        {{81, 12}, {0, 2, 4}},
        {{13,  5}, {0, 3, 1}},
        {{35,  9}, {0, 3, 2}},
        {{13, 10}, {0, 3, 3}},
        {{12,  5}, {0, 4, 1}},
        {{34,  9}, {0, 4, 2}},
        {{82, 12}, {0, 4, 3}},
        {{11,  5}, {0, 5, 1}},
        {{12, 10}, {0, 5, 2}},
        {{83, 12}, {0, 5, 3}},
        {{19,  6}, {0, 6, 1}},
        {{11, 10}, {0, 6, 2}},
        {{84, 12}, {0, 6, 3}},
        {{18,  6}, {0, 7, 1}},
        {{10, 10}, {0, 7, 2}},
        {{17,  6}, {0, 8, 1}},
        {{ 9, 10}, {0, 8, 2}},
        {{16,  6}, {0, 9, 1}},
        {{ 8, 10}, {0, 9, 2}},
        {{22,  7}, {0, 10, 1}},
        {{85, 12}, {0, 10, 2}},
        {{21,  7}, {0, 11, 1}},
        {{20,  7}, {0, 12, 1}},
        {{28,  8}, {0, 13, 1}},
        {{27,  8}, {0, 14, 1}},
        {{33,  9}, {0, 15, 1}},
        {{32,  9}, {0, 16, 1}},
        {{31,  9}, {0, 17, 1}},
        {{30,  9}, {0, 18, 1}},
        {{29,  9}, {0, 19, 1}},
        {{28,  9}, {0, 20, 1}},
        {{27,  9}, {0, 21, 1}},
        {{26,  9}, {0, 22, 1}},
        {{34, 11}, {0, 23, 1}},
        {{35, 11}, {0, 24, 1}},
        {{86, 12}, {0, 25, 1}},
        {{87, 12}, {0, 26, 1}},
        {{ 7,  4}, {1, 0, 1}},
        {{25,  9}, {1, 0, 2}},
        {{ 5, 11}, {1, 0, 3}},
        {{15,  6}, {1, 1, 1}},
        {{ 4, 11}, {1, 1, 2}},
        {{14,  6}, {1, 2, 1}},
        {{13,  6}, {1, 3, 1}},
        {{12,  6}, {1, 4, 1}},
        {{19,  7}, {1, 5, 1}},
        {{18,  7}, {1, 6, 1}},
        {{17,  7}, {1, 7, 1}},
        {{16,  7}, {1, 8, 1}},
        {{26,  8}, {1, 9, 1}},
        {{25,  8}, {1, 10, 1}},
        {{24,  8}, {1, 11, 1}},
        {{23,  8}, {1, 12, 1}},
        {{22,  8}, {1, 13, 1}},
        {{21,  8}, {1, 14, 1}},
        {{20,  8}, {1, 15, 1}},
        {{19,  8}, {1, 16, 1}},
        {{24,  9}, {1, 17, 1}},
        {{23,  9}, {1, 18, 1}},
        {{22,  9}, {1, 19, 1}},
        {{21,  9}, {1, 20, 1}},
        {{20,  9}, {1, 21, 1}},
        {{19,  9}, {1, 22, 1}},
        {{18,  9}, {1, 23, 1}},
        {{17,  9}, {1, 24, 1}},
        {{ 7, 10}, {1, 25, 1}},
        {{ 6, 10}, {1, 26, 1}},
        {{ 5, 10}, {1, 27, 1}},
        {{ 4, 10}, {1, 28, 1}},
        {{36, 11}, {1, 29, 1}},
        {{37, 11}, {1, 30, 1}},
        {{38, 11}, {1, 31, 1}},
        {{39, 11}, {1, 32, 1}},
        {{88, 12}, {1, 33, 1}},
        {{89, 12}, {1, 34, 1}},
        {{90, 12}, {1, 35, 1}},
        {{91, 12}, {1, 36, 1}},
        {{92, 12}, {1, 37, 1}},
        {{93, 12}, {1, 38, 1}},
        {{94, 12}, {1, 39, 1}},
        {{95, 12}, {1, 40, 1}}
    },
    /* intra = 1 */
    {
        {{ 2,  2}, {0, 0, 1}},
        {{15,  4}, {0, 0, 3}},
        {{21,  6}, {0, 0, 6}},
        {{23,  7}, {0, 0, 9}},
        {{31,  8}, {0, 0, 10}},
        {{37,  9}, {0, 0, 13}},
        {{36,  9}, {0, 0, 14}},
        {{33, 10}, {0, 0, 17}},
        {{32, 10}, {0, 0, 18}},
        {{ 7, 11}, {0, 0, 21}},
        {{ 6, 11}, {0, 0, 22}},
        {{32, 11}, {0, 0, 23}},
        {{ 6,  3}, {0, 0, 2}},
        {{20,  6}, {0, 1, 2}},
        {{30,  8}, {0, 0, 11}},
        {{15, 10}, {0, 0, 19}},
        {{33, 11}, {0, 0, 24}},
        {{80, 12}, {0, 0, 25}},
        {{14,  4}, {0, 1, 1}},
        {{29,  8}, {0, 0, 12}},
        {{14, 10}, {0, 0, 20}},
        {{81, 12}, {0, 0, 26}},
        {{13,  5}, {0, 0, 4}},
        {{35,  9}, {0, 0, 15}},
        {{13, 10}, {0, 1, 7}},
        {{12,  5}, {0, 0, 5}},
        {{34,  9}, {0, 4, 2}},
        {{82, 12}, {0, 0, 27}},
        {{11,  5}, {0, 2, 1}},
        {{12, 10}, {0, 2, 4}},
        {{83, 12}, {0, 1, 9}},
        {{19,  6}, {0, 0, 7}},
        {{11, 10}, {0, 3, 4}},
        {{84, 12}, {0, 6, 3}},
        {{18,  6}, {0, 0, 8}},
        {{10, 10}, {0, 4, 3}},
        {{17,  6}, {0, 3, 1}},
        {{ 9, 10}, {0, 8, 2}},
        {{16,  6}, {0, 4, 1}},
        {{ 8, 10}, {0, 5, 3}},
        {{22,  7}, {0, 1, 3}},
        {{85, 12}, {0, 1, 10}},
        {{21,  7}, {0, 2, 2}},
        {{20,  7}, {0, 7, 1}},
        {{28,  8}, {0, 1, 4}},
        {{27,  8}, {0, 3, 2}},
        {{33,  9}, {0, 0, 16}},
        {{32,  9}, {0, 1, 5}},
        {{31,  9}, {0, 1, 6}},
        {{30,  9}, {0, 2, 3}},
        {{29,  9}, {0, 3, 3}},
        {{28,  9}, {0, 5, 2}},
        {{27,  9}, {0, 6, 2}},
        {{26,  9}, {0, 7, 2}},
        {{34, 11}, {0, 1, 8}},
        {{35, 11}, {0, 9, 2}},
        {{86, 12}, {0, 2, 5}},
        {{87, 12}, {0, 7, 3}},
        {{ 7,  4}, {1, 0, 1}},
        {{25,  9}, {0, 11, 1}},
        {{ 5, 11}, {1, 0, 6}},
        {{15,  6}, {1, 1, 1}},
        {{ 4, 11}, {1, 0, 7}},
        {{14,  6}, {1, 2, 1}},
        {{13,  6}, {0, 5, 1}},
        {{12,  6}, {1, 0, 2}},
        {{19,  7}, {1, 5, 1}},
        {{18,  7}, {0, 6, 1}},
        {{17,  7}, {1, 3, 1}},
        {{16,  7}, {1, 4, 1}},
        {{26,  8}, {1, 9, 1}},
        {{25,  8}, {0, 8, 1}},
        {{24,  8}, {0, 9, 1}},
        {{23,  8}, {0, 10, 1}},
        {{22,  8}, {1, 0, 3}},
        {{21,  8}, {1, 6, 1}},
        {{20,  8}, {1, 7, 1}},
        {{19,  8}, {1, 8, 1}},
        {{24,  9}, {0, 12, 1}},
        {{23,  9}, {1, 0, 4}},
        {{22,  9}, {1, 1, 2}},
        {{21,  9}, {1, 10, 1}},
        {{20,  9}, {1, 11, 1}},
        {{19,  9}, {1, 12, 1}},
        {{18,  9}, {1, 13, 1}},
        {{17,  9}, {1, 14, 1}},
        {{ 7, 10}, {0, 13, 1}},
        {{ 6, 10}, {1, 0, 5}},
        {{ 5, 10}, {1, 1, 3}},
        {{ 4, 10}, {1, 2, 2}},
        {{36, 11}, {1, 3, 2}},
        {{37, 11}, {1, 4, 2}},
        {{38, 11}, {1, 15, 1}},
        {{39, 11}, {1, 16, 1}},
        {{88, 12}, {0, 14, 1}},
        {{89, 12}, {1, 0, 8}},
        {{90, 12}, {1, 5, 2}},
        {{91, 12}, {1, 6, 2}},
        {{92, 12}, {1, 17, 1}},
        {{93, 12}, {1, 18, 1}},
        {{94, 12}, {1, 19, 1}},
        {{95, 12}, {1, 20, 1}}
    }
};

/* constants taken from momusys/vm_common/inlcude/max_level.h */
uint8_t const max_level[2][2][64] =
{
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
    },
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
    }
};

uint8_t const max_run[2][2][64] =
{
    {
        /* intra = 0, last = 0 */
        {
            0, 26, 10, 6, 2, 1, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        /* intra = 0, last = 1 */
        {
            0, 40, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        }
    },
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
    }
};

//#endif
/******************************************************************
 * decoder tables                                                 *
 ******************************************************************/

VLC const mcbpc_intra_table[64] =
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

VLC const mcbpc_inter_table[257] =
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

VLC const cbpy_table[64] =
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

VLC const TMNMVtab0[] =
{
    {3, 4}, {-3, 4}, {2, 3}, {2, 3}, {-2, 3}, {-2, 3}, {1, 2},
    {1, 2}, {1, 2}, {1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}
};

VLC const TMNMVtab1[] =
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

VLC const TMNMVtab2[] =
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

VLC const dc_lum_tab[] =
{
    {0, 0}, {4, 3}, {3, 3}, {0, 3},
    {2, 2}, {2, 2}, {1, 2}, {1, 2},
};


//for decoder.c below
//add by longyl
const uint8_t MPEG_DC_SCALAR[2][32] =
{
    {
        0, 8, 8, 8, 8, //0-4
        9, 9, 10, 10, 11, //5-9
        11, 12, 12, 13, 13, //10-14
        14, 14, 15, 15, 16, //15-19
        16, 17, 17, 18, 18, //20-24
        19, 20, 21, 22, 23, //25-29
        24, 25              //30-32
    },
    {
        0, 8, 8, 8, 8, //0-4
        10, 12, 14, 16, 17, //5-9
        18, 19, 20, 21, 22, //10-14
        23, 24, 25, 26, 27, //15-19
        28, 29, 30, 31, 32, //20-24
        34, 36, 38, 40, 42, //25-29
        44, 46
    }              //30-32
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

const uint16_t iQuant_MULT[32] =
{
    0x0000,0x8001,0x4001,0x2aab,
    0x2001,0x199a,0x1556,0x124a,
    0x1001,0x0e39,0x0ccd,0x0ba3,
    0x0aab,0x09d9,0x0925,0x0889,

    0x0801,0x0788,0x071d,0x06bd,
    0x0667,0x0619,0x05d2,0x0591,
    0x0556,0x051f,0x04ed,0x04be,
    0x0493,0x046a,0x0445,0x0422
};


/*****************************************************************************
 * XviD Init Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 * Returned value : XVID_ERR_OK
 *                  + API_VERSION in the input XVID_INIT_PARAM structure
 *                  + core build  "   "    "       "               "
 *
 ****************************************************************************/


static
int xvid_gbl_init(xvid_gbl_init_t * init)
{
    if (XVID_VERSION_MAJOR(init->version) != 1) /* v1.x.x */
        return XVID_ERR_VERSION;

//#ifdef NOACC
    /* Initialize the function pointers */
    if(idct_int32_init())
        return XVID_ERR_MEMORY;
    init_vlc_tables();

    /* Fixed Point Forward/Inverse DCT transformations */
    //idct = idct_int32;
    /* Quantization functions */

    dequant_h263_intra = dequant_h263_intra_c;
    dequant_h263_inter = dequant_h263_inter_c;
//#endif

    /* Block transfer related functions */
    transfer_16to8copy = transfer_16to8copy_c;
    transfer_16to8add  = transfer_16to8add_c;

    return(0);
}

/*****************************************************************************
 * XviD Global Entry point
 *
 * Well this function initialize all internal function pointers according
 * to the CPU features forced by the library client or autodetected (depending
 * on the XVID_CPU_FORCE flag). It also initializes vlc coding tables and all
 * image colorspace transformation tables.
 *
 ****************************************************************************/


int
xvid_global(void *handle,
            int opt,
            void *param1,
            void *param2)
{
    switch(opt)
    {
        case XVID_GBL_INIT :
            return xvid_gbl_init((xvid_gbl_init_t*)param1);

        default :
            return XVID_ERR_FAIL;
    }
}

/*****************************************************************************
 * XviD Native decoder entry point
 *
 * This function is just a wrapper to all the option cases.
 *
 * Returned values : XVID_ERR_FAIL when opt is invalid
 *                   else returns the wrapped function result
 *
 ****************************************************************************/

int
xvid_decore(void *handle,
            int opt,
            void *param1,
            void *param2)
{
    switch (opt)
    {
        case XVID_DEC_CREATE:
            return decoder_create((xvid_dec_create_t *) param1);

        case XVID_DEC_DESTROY:
            return decoder_destroy((DECODER *) handle);

        case XVID_DEC_DECODE:
            return decoder_decode((DECODER *) handle, (xvid_dec_frame_t *) param1, (xvid_dec_stats_t*) param2);

        default:
            return XVID_ERR_FAIL;
    }
}

#if 1//for mbcoding.c
void
init_vlc_tables(void)
{
    uint32_t i, j, intra, last, run,  run_esc, level, level_esc, escape, escape_len, offset;
    //int32_t l;
    //uint32_t k;

    for (intra = 0; intra < 2; intra++)
        for (i = 0; i < 4096; i++)
            DCT3D[intra][i].event.level = 0;

    for (intra = 0; intra < 2; intra++)
    {
        for (last = 0; last < 2; last++)
        {
            for (run = 0; run < 63 + last; run++)
            {
                for (level = 0; level < (uint32_t)(32 << intra); level++)
                {
                    offset = !intra * LEVELOFFSET;
                    coeff_VLC[intra][last][level + offset][run].len = 128;
                }
            }
        }
    }

    for (intra = 0; intra < 2; intra++)
    {
        for (i = 0; i < 102; i++)
        {
            offset = !intra * LEVELOFFSET;

            for (j = 0; j < (uint32_t)(1 << (12 - coeff_tab[intra][i].vlc.len)); j++)
            {
                DCT3D[intra][(coeff_tab[intra][i].vlc.code << (12 - coeff_tab[intra][i].vlc.len)) | j].len   = coeff_tab[intra][i].vlc.len;
                DCT3D[intra][(coeff_tab[intra][i].vlc.code << (12 - coeff_tab[intra][i].vlc.len)) | j].event = coeff_tab[intra][i].event;
            }

            coeff_VLC[intra][coeff_tab[intra][i].event.last][coeff_tab[intra][i].event.level + offset][coeff_tab[intra][i].event.run].code
                = coeff_tab[intra][i].vlc.code << 1;
            coeff_VLC[intra][coeff_tab[intra][i].event.last][coeff_tab[intra][i].event.level + offset][coeff_tab[intra][i].event.run].len
                = coeff_tab[intra][i].vlc.len + 1;

            if (!intra)
            {
                coeff_VLC[intra][coeff_tab[intra][i].event.last][offset - coeff_tab[intra][i].event.level][coeff_tab[intra][i].event.run].code
                    = (coeff_tab[intra][i].vlc.code << 1) | 1;
                coeff_VLC[intra][coeff_tab[intra][i].event.last][offset - coeff_tab[intra][i].event.level][coeff_tab[intra][i].event.run].len
                    = coeff_tab[intra][i].vlc.len + 1;
            }
        }
    }

    for (intra = 0; intra < 2; intra++)
    {
        for (last = 0; last < 2; last++)
        {
            for (run = 0; run < 63 + last; run++)
            {
                for (level = 1; level < (uint32_t)(32 << intra); level++)
                {

                    if (level <= max_level[intra][last][run] && run <= max_run[intra][last][level])
                        continue;

                    offset = !intra * LEVELOFFSET;
                    level_esc = level - max_level[intra][last][run];
                    run_esc = run - 1 - max_run[intra][last][level];

                    if (level_esc <= max_level[intra][last][run] && run <= max_run[intra][last][level_esc])
                    {
                        escape     = ESCAPE1;
                        escape_len = 7 + 1;
                        run_esc    = run;
                    }
                    else
                    {
                        if (run_esc <= max_run[intra][last][level] && level <= max_level[intra][last][run_esc])
                        {
                            escape     = ESCAPE2;
                            escape_len = 7 + 2;
                            level_esc  = level;
                        }
                        else
                        {
                            if (!intra)
                            {
                                coeff_VLC[intra][last][level + offset][run].code
                                    = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((level & 0xfff) << 1) | 1;
                                coeff_VLC[intra][last][level + offset][run].len = 30;
                                coeff_VLC[intra][last][offset - level][run].code
                                    = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((-(int32_t)level & 0xfff) << 1) | 1;
                                coeff_VLC[intra][last][offset - level][run].len = 30;
                            }
                            continue;
                        }
                    }

                    coeff_VLC[intra][last][level + offset][run].code
                        = (escape << coeff_VLC[intra][last][level_esc + offset][run_esc].len)
                          |  coeff_VLC[intra][last][level_esc + offset][run_esc].code;
                    coeff_VLC[intra][last][level + offset][run].len
                        = coeff_VLC[intra][last][level_esc + offset][run_esc].len + escape_len;

                    if (!intra)
                    {
                        coeff_VLC[intra][last][offset - level][run].code
                            = (escape << coeff_VLC[intra][last][level_esc + offset][run_esc].len)
                              |  coeff_VLC[intra][last][level_esc + offset][run_esc].code | 1;
                        coeff_VLC[intra][last][offset - level][run].len
                            = coeff_VLC[intra][last][level_esc + offset][run_esc].len + escape_len;
                    }
                }

                if (!intra)
                {
                    coeff_VLC[intra][last][0][run].code
                        = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((-32 & 0xfff) << 1) | 1;
                    coeff_VLC[intra][last][0][run].len = 30;
                }
            }
        }
    }
}
#endif

#if 1 //for decoder.c
extern void
decoder_iframe(DECODER * dec,
               Bitstream * bs,
               int quant,
               int intra_dc_threshold);

extern void
decoder_pframe(DECODER * dec,
               Bitstream * bs,
               int rounding,
               int quant,
               int fcode,
               int intra_dc_threshold);

/* perform post processing if necessary, and output the image */
static void decoder_output(DECODER * dec, IMAGE * img,xvid_dec_frame_t * frame, xvid_dec_stats_t * stats,
                           int coding_type)
{
//  frame->general = 1;
    image_output(img, dec->width, dec->height,
                 dec->edged_width, (uint8_t**)frame->output.plane);

    if (stats)
    {
        stats->type = coding2type(coding_type);
        stats->data.vop.time_base = (int)dec->time_base;
        stats->data.vop.time_increment = 0; /* XXX: todo */
        stats->data.vop.qscale_stride = dec->mb_width;
    }
}

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
        goto memory_error;

    /* nothing happens if that fails */
    temp_size = sizeof(int16_t) * (dec->mb_width + 2);
    dec->qscale =
        xvid_malloc(temp_size, CACHE_LINE);
    if (dec->qscale)
        memset(dec->qscale, 0, temp_size);
    dec->qscale_current = dec->qscale;
    dec->qscale_above = dec->qscale + 1;
    dec->qscale_left = dec->qscale + dec->mb_width + 1;

    temp_size = (sizeof(int16_t) * (dec->mb_width+2)) << 5;
    dec->ac_pred_base = (int16_t *)xvid_malloc(temp_size,CACHE_LINE);
    if(dec->ac_pred_base)
        memset(dec->ac_pred_base,0,temp_size);
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

    temp_size = sizeof(int16_t) * 384;
    dec->block_size = temp_size;
    dec->block = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->block)
        memset(dec->block,0,temp_size);

    dec->data = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->data)
        memset(dec->data,0,temp_size);
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


int
decoder_decode(DECODER * dec,
               xvid_dec_frame_t * frame, xvid_dec_stats_t * stats)
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
            image_copy(&dec->cur, &dec->refn[0], dec->edged_width, dec->height);
            break;
    }
#if 0
    if (dec->frames > 0)   /* is the reference frame valid? */
    {
        /* output the reference frame */
        decoder_output(dec, &dec->refn[0], frame, stats, dec->last_coding_type);
        output = 1;
    }
#else// output current decode frame
    decoder_output(dec, &dec->cur, frame, stats, coding_type);
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

#if 1//for meped4_bitstream.c

#define VIDOBJLAY_START_CODE_MASK   0x0000000f
extern const uint32_t intra_dc_threshold_table[] ;
extern uint32_t __inline log2bin(uint32_t value);

int
BitstreamReadHeaders(Bitstream * bs,
                     DECODER * dec,
                     uint16_t * rounding,
                     uint16_t * quant,
                     uint16_t * fcode_forward,
                     uint16_t * intra_dc_threshold)
{
    uint16_t vol_ver_id;
    uint16_t coding_type;
    uint32_t start_code;
    uint32_t temp_code;
    uint32_t time_incr = 0;
    int32_t time_increment = 0;
    int resize = 0;

    while ((BitstreamPos(bs) >> 3) + 4 <= bs->length)
    {

        _BITSTREAMBYTEALIGN(bs->pos);
        _SHOWBITS(bs->buf,bs->pos,32,start_code);

        if (start_code == VISOBJSEQ_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            _GETBITS(bs->buf,bs->pos,8,temp_code);
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
            /*      if(temp_code != XVID_PROFILE_S_L0 && temp_code != XVID_PROFILE_S_L1 &&
                         temp_code != XVID_PROFILE_S_L2 && temp_code != XVID_PROFILE_S_L3)//profile unfit
                         return -8;*/
        }
        else if ((start_code & ~VIDOBJLAY_START_CODE_MASK) == VIDOBJLAY_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            bs->pos -= 9;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* is_object_layer_identifier */
            {
                _GETBITS(bs->buf,bs->pos,4,vol_ver_id);
                bs->pos -= 3;
            }
            else
            {
                vol_ver_id = dec->ver_id;
            }
            _GETBITS(bs->buf,bs->pos,4,dec->aspect_ratio);
            if (dec->aspect_ratio == VIDOBJLAY_AR_EXTPAR)   /* aspect_ratio_info */
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,8,dec->par_width);
                _GETBITS(bs->buf,bs->pos,8,dec->par_height);
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            if (temp_code)  /* vol_control_parameters */
            {
                bs->pos -= 2;
                _GETBITS(bs->buf,bs->pos,1,dec->low_delay);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (temp_code)  /* vbv_parameters */
                {
                    unsigned int bitrate;
                    unsigned int buffer_size;
                    unsigned int occupancy;

                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,bitrate);
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    temp_code = bitrate << 15;
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,bitrate);
                    bitrate |= temp_code;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,buffer_size);
                    buffer_size <<= 3;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,3,temp_code);
                    buffer_size |= temp_code;
                    _GETBITS(bs->buf,bs->pos,11,occupancy);
                    occupancy <<= 15;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,temp_code);
                    occupancy |= temp_code;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                }
            }
            else
            {
                dec->low_delay = dec->low_delay_default;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,2,dec->shape);
            if (dec->shape != VIDOBJLAY_SHAPE_RECTANGULAR)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            /********************** for decode B-frame time ***********************/
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,16,dec->time_inc_resolution);
            if (dec->time_inc_resolution > 0)
            {
                dec->time_inc_bits = MAX_16(log2bin(dec->time_inc_resolution-1), 1);
            }
            else
            {
                /* for "old" xvid compatibility, set time_inc_bits = 1 */
                dec->time_inc_bits = 1;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* fixed_vop_rate */
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                bs->pos -= dec->time_inc_bits;
            }
            {
                uint32_t width, height;
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                _GETBITS(bs->buf,bs->pos,13,width);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,13,height);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (dec->width != width || dec->height != height)
                {
                    if (dec->fixed_dimensions)
                    {
                        return -1;
                    }
                    resize = 1;
                    dec->width = width;
                    dec->height = height;
                }
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if(temp_code)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code) /* obmc_disable */
            {
                return -1;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,(vol_ver_id == 1 ? 1 : 2),temp_code);
            if (temp_code == SPRITE_STATIC || temp_code == SPRITE_GMC)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* not_8_bit */
            {
                return -1;
            }
            dec->quant_bits = 5;
            _GETBITS(bs->buf,bs->pos,1,dec->quant_type);
            if (vol_ver_id != 1)
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(temp_code)
                {
                    return -1;
                }

            }
            else
                temp_code = 0;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code)
            {
                return -1;
            }
            bs->pos --;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* data_partitioned */
            {
                return -1;
            }

            if (vol_ver_id != 1)
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (temp_code)  /* newpred_enable */
                {
                    return -1;
                }
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(temp_code)
                {
                    return -1;
                }
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)
            {
                return -1;
            }
            return (resize ? -3 : -2 ); /* VOL */

        } /*else if (start_code == GRPOFVOP_START_CODE) {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            {
                int hours, minutes, seconds;
                _GETBITS(bs->buf,bs->pos,5,hours);
                _GETBITS(bs->buf,bs->pos,1,minutes);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                _GETBITS(bs->buf,bs->pos,1,seconds);
            }
            bs->pos -= 2;
        }*/ else if (start_code == VOP_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            _GETBITS(bs->buf,bs->pos,2,coding_type);
            /*********************** for decode B-frame time ***********************/
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            while (temp_code != 0)  /* time_base */
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                time_incr++;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (dec->time_inc_bits)
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,dec->time_inc_bits,time_increment);
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
//          dec->last_time_base = dec->time_base;
            dec->time_base += time_incr;
            dec->time = dec->time_base*dec->time_inc_resolution + time_increment;
            //      dec->time_pp = (int32_t)(dec->time - dec->last_non_b_time);
            //      dec->last_non_b_time = dec->time;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code) /* vop_coded */
            {
                return N_VOP;
            }
            /* fix a little bug by MinChen <chenm002@163.com> */
            if (coding_type == P_VOP)
            {
                _GETBITS(bs->buf,bs->pos,1,*rounding);
            }
            /* intra_dc_vlc_threshold */
            _GETBITS(bs->buf,bs->pos,3,temp_code);
            *intra_dc_threshold =
                intra_dc_threshold_table[temp_code];

            _GETBITS(bs->buf,bs->pos,dec->quant_bits,*quant);
            if (coding_type != I_VOP)
            {
                _GETBITS(bs->buf,bs->pos,3,*fcode_forward);
            }
            return coding_type;
        }
        else                    /* start_code == ? */
        {
            bs->pos -= 8;
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos)
        }
    }
    return -1;                  /* ignore it */
}
#endif

