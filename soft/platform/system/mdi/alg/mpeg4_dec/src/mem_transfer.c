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
#endif
#ifdef MP4_3GP_SUPPORT

#ifndef MEDIA_VOCVID_SUPPORT
#include "mpeg4_global.h"
#include "mem_transfer.h"

/* Function pointers - Initialized in the xvid.c module */
TRANSFER_16TO8COPY_PTR transfer_16to8copy;
TRANSFER_16TO8ADD_PTR  transfer_16to8add;
//TRANSFER8X8_COPY_PTR transfer8x8_copy;

#define USE_REFERENCE_C
const static uint8_t trans_sat_table[384] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,

    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,

    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
static uint8_t *trans_sat_base = trans_sat_table + 64;
//test by longyl
//extern FILE *file_test;
//extern int32_t macroblock_number;
////////////////////
/*****************************************************************************
 *
 * All these functions are used to transfer data from a 8 bit data array
 * to a 16 bit data array.
 *
 * This is typically used during motion compensation, that's why some
 * functions also do the addition/substraction of another buffer during the
 * so called  transfer.
 *
 ****************************************************************************/
//static uint32_t t=0,t1=0,m=0,t2=0,n=0;//test
/*
 * SRC - the source buffer
 * DST - the destination buffer
 *
 * Then the function does the 8->16 bit transfer and this serie of operations :
 *
 *    SRC (16bit) = SRC
 *    DST (8bit)  = max(min(SRC, 255), 0)
 */

#if 1
void
transfer_16to8copy_c(uint8_t * dst,
                     int16_t * src,
                     uint16_t stride)
{
    int j;
    uint8_t *dst_tmp = dst;
    int16_t *src_tmp = src;

    for (j = 8; j > 0; j--)
    {
        dst_tmp[0] = trans_sat_base[src_tmp[0]];
        dst_tmp[1] = trans_sat_base[src_tmp[1]];
        dst_tmp[2] = trans_sat_base[src_tmp[2]];
        dst_tmp[3] = trans_sat_base[src_tmp[3]];
        dst_tmp[4] = trans_sat_base[src_tmp[4]];
        dst_tmp[5] = trans_sat_base[src_tmp[5]];
        dst_tmp[6] = trans_sat_base[src_tmp[6]];
        dst_tmp[7] = trans_sat_base[src_tmp[7]];
        dst_tmp += stride;
        src_tmp += 8;
    }
}
#else


void
transfer_16to8copy_c(uint8_t * dst,
                     int16_t * src,
                     uint16_t stride)
{
    int j;
    uint32_t dst_temp;
//time_measure_start();
    for (j = 8; j > 0; j--)
    {
        dst_temp = trans_sat_base[src[0]];
        dst_temp += trans_sat_base[src[1]] << 8;
        dst_temp += trans_sat_base[src[2]] << 16;
        dst_temp += trans_sat_base[src[3]] << 24;
        *(uint32_t *)&dst[0] = dst_temp;
        dst_temp =  trans_sat_base[src[4]];

        dst_temp += trans_sat_base[src[5]] << 8;
        dst_temp += trans_sat_base[src[6]] << 16;
        dst_temp += trans_sat_base[src[7]] << 24;
        *(uint32_t *)&dst[4] = dst_temp;
        dst += stride;
        src += 8;
    }
    /*
    t=time_measure_end();
    t1+=t;
    m+=1;
    diag_printf("t1=%d m=%d\n",t1,m);
    */
}
#endif

/*
 * SRC - the source buffer
 * DST - the destination buffer
 *
 * Then the function does the 16->8 bit transfer and this serie of operations :
 *
 *    SRC (16bit) = SRC
 *    DST (8bit)  = max(min(DST+SRC, 255), 0)
 */
#if 1
void
transfer_16to8add_c(uint8_t * dst,
                    int16_t * src,
                    uint32_t stride)
{
    int j;

    for(j = 8; j > 0; j--)
    {
        dst[0] = trans_sat_base[dst[0] + src[0]];
        dst[1] = trans_sat_base[dst[1] + src[1]];
        dst[2] = trans_sat_base[dst[2] + src[2]];
        dst[3] = trans_sat_base[dst[3] + src[3]];

        dst[4] = trans_sat_base[dst[4] + src[4]];
        dst[5] = trans_sat_base[dst[5] + src[5]];
        dst[6] = trans_sat_base[dst[6] + src[6]];
        dst[7] = trans_sat_base[dst[7] + src[7]];

        dst += 8;
        src += 8;
    }
}

#else
void
transfer_16to8add_c(uint8_t * dst,
                    int16_t * src,
                    uint32_t stride)
{
    int j;
    uint32_t dst_temp;
//time_measure_start();
    for(j = 8; j > 0; j--)
    {
        dst_temp = trans_sat_base[dst[0] + src[0]];
        dst_temp += trans_sat_base[dst[1] + src[1]] << 8;
        dst_temp +=  trans_sat_base[dst[2] + src[2]] << 16;
        dst_temp +=  trans_sat_base[dst[3] + src[3]] << 24;
        *(uint32_t *)&dst[0] = dst_temp;

        dst_temp =  trans_sat_base[dst[4] + src[4]];
        dst_temp +=  trans_sat_base[dst[5] + src[5]] << 8;
        dst_temp +=  trans_sat_base[dst[6] + src[6]] << 16;
        dst_temp +=  trans_sat_base[dst[7] + src[7]] << 24;
        *(uint32_t *)&dst[4] = dst_temp;
        dst += 8;
        src += 8;
    }
    /*
    t=time_measure_end();
    t2+=t;
    n+=1;
    diag_printf("t2=%d n=%d\n",t2,n);
    */
}
#endif

#endif// MEDIA_VOCVID_SUPPORT
#endif

