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


#ifndef MEDIA_VOCVID_SUPPORT
#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif
#ifdef MP4_3GP_SUPPORT

#include "portab.h"
#include "mpeg4_global.h"
#include "interpolate8x8.h"

#define         BYTE_VEC32(c)   ((c)*0x01010101UL)
#define AV_RN32(a) (*((const uint32_t*)(a)))
#define AV_WN32(a, b) *((uint32_t*)(a)) = (b)

#define     BYTE_LIMT(a)  ((a<0xff)?(a):(0xff))

#ifndef VOC_DO
void
transfer16x16_copy_c(uint8_t *dst,uint8_t *ref,int32_t stride,
                     int32_t rounding)
{
    int16_t i;

    uint8_t ref_diff = (uint32_t)ref & 3;
    switch(ref_diff)
    {
        case 0:
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
            uint32_t *ref_addr_stride = (uint32_t *)(ref + (stride << 3));
            uint32_t *dst_addr_tmp = (uint32_t *)dst;
            uint32_t stride_tmp = (stride >> 2);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];
                dst_addr_tmp[16] = ref_addr_tmp0[2];
                dst_addr_tmp[17] = ref_addr_tmp0[3];

                dst_addr_tmp[32] = ref_addr_stride[0];
                dst_addr_tmp[33] = ref_addr_stride[1];
                dst_addr_tmp[48] = ref_addr_stride[2];
                dst_addr_tmp[49] = ref_addr_stride[3];

                dst_addr_tmp += 2;
                ref_addr_tmp0 += stride_tmp;
                ref_addr_stride += stride_tmp;
            }
        }
        break;
        case 2:
        {
            uint16_t *ref_16 = (uint16_t *)ref;
            uint16_t *ref_stride = (uint16_t *)(ref + (stride<<3));
            uint16_t *dst_16 = (uint16_t *)dst;
            int32_t stride_tmp = stride >> 1;

            for(i = 8; i > 0; i--)
            {
                dst_16[0] = ref_16[0];
                dst_16[1] = ref_16[1];
                dst_16[2] = ref_16[2];
                dst_16[3] = ref_16[3];

                dst_16[32] = ref_16[4];
                dst_16[33] = ref_16[5];
                dst_16[34] = ref_16[6];
                dst_16[35] = ref_16[7];

                dst_16[64] = ref_stride[0];
                dst_16[65] = ref_stride[1];
                dst_16[66] = ref_stride[2];
                dst_16[67] = ref_stride[3];

                dst_16[96] = ref_stride[4];
                dst_16[97] = ref_stride[5];
                dst_16[98] = ref_stride[6];
                dst_16[99] = ref_stride[7];

                dst_16 += 4;
                ref_16 += stride_tmp;
                ref_stride += stride_tmp;
            }
        }
        break;
        default:
        {
            uint8_t *ref_stride = ref + (stride<<3);

            for(i = 8; i > 0; i--)
            {
                dst[0] = ref[0];
                dst[1] = ref[1];
                dst[2] = ref[2];
                dst[3] = ref[3];

                dst[4] = ref[4];
                dst[5] = ref[5];
                dst[6] = ref[6];
                dst[7] = ref[7];

                dst[64] = ref[8];
                dst[65] = ref[9];
                dst[66] = ref[10];
                dst[67] = ref[11];

                dst[68] = ref[12];
                dst[69] = ref[13];
                dst[70] = ref[14];
                dst[71] = ref[15];

                dst[128] = ref_stride[0];
                dst[129] = ref_stride[1];
                dst[130] = ref_stride[2];
                dst[131] = ref_stride[3];

                dst[132] = ref_stride[4];
                dst[133] = ref_stride[5];
                dst[134] = ref_stride[6];
                dst[135] = ref_stride[7];

                dst[192] = ref_stride[8];
                dst[193] = ref_stride[9];
                dst[194] = ref_stride[10];
                dst[195] = ref_stride[11];

                dst[196] = ref_stride[12];
                dst[197] = ref_stride[13];
                dst[198] = ref_stride[14];
                dst[199] = ref_stride[15];

                dst += 8;
                ref += stride;
                ref_stride += stride;
            }
        }
        break;
    }
}

void
interpolate16x16_halfpel_v_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 0
    uint8_t *ref_stride = ref + stride;
    uint8_t *ref_stride8 = ref + (stride << 3);
    uint8_t *ref_stride9 = ref_stride8 + stride;

    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref_stride[0] + rounding) >> 1;
        dst[1] = (ref[1] + ref_stride[1] + rounding) >> 1;
        dst[2] = (ref[2] + ref_stride[2] + rounding) >> 1;
        dst[3] = (ref[3] + ref_stride[3] + rounding) >> 1;

        dst[4] = (ref[4] + ref_stride[4] + rounding) >> 1;
        dst[5] = (ref[5] + ref_stride[5] + rounding) >> 1;
        dst[6] = (ref[6] + ref_stride[6] + rounding) >> 1;
        dst[7] = (ref[7] + ref_stride[7] + rounding) >> 1;

        dst[64] = (ref[8] + ref_stride[8] + rounding) >> 1;
        dst[65] = (ref[9] + ref_stride[9] + rounding) >> 1;
        dst[66] = (ref[10] + ref_stride[10] + rounding) >> 1;
        dst[67] = (ref[11] + ref_stride[11] + rounding) >> 1;

        dst[68] = (ref[12] + ref_stride[12] + rounding) >> 1;
        dst[69] = (ref[13] + ref_stride[13] + rounding) >> 1;
        dst[70] = (ref[14] + ref_stride[14] + rounding) >> 1;
        dst[71] = (ref[15] + ref_stride[15] + rounding) >> 1;

        dst[128] = (ref_stride8[0] + ref_stride9[0] + rounding) >> 1;
        dst[129] = (ref_stride8[1] + ref_stride9[1] + rounding) >> 1;
        dst[130] = (ref_stride8[2] + ref_stride9[2] + rounding) >> 1;
        dst[131] = (ref_stride8[3] + ref_stride9[3] + rounding) >> 1;

        dst[132] = (ref_stride8[4] + ref_stride9[4] + rounding) >> 1;
        dst[133] = (ref_stride8[5] + ref_stride9[5] + rounding) >> 1;
        dst[134] = (ref_stride8[6] + ref_stride9[6] + rounding) >> 1;
        dst[135] = (ref_stride8[7] + ref_stride9[7] + rounding) >> 1;

        dst[192] = (ref_stride8[8] + ref_stride9[8] + rounding) >> 1;
        dst[193] = (ref_stride8[9] + ref_stride9[9] + rounding) >> 1;
        dst[194] = (ref_stride8[10] + ref_stride9[10] + rounding) >> 1;
        dst[195] = (ref_stride8[11] + ref_stride9[11] + rounding) >> 1;

        dst[196] = (ref_stride8[12] + ref_stride9[12] + rounding) >> 1;
        dst[197] = (ref_stride8[13] + ref_stride9[13] + rounding) >> 1;
        dst[198] = (ref_stride8[14] + ref_stride9[14] + rounding) >> 1;
        dst[199] = (ref_stride8[15] + ref_stride9[15] + rounding) >> 1;

        dst += 8;
        ref += stride;
        ref_stride += stride;
        ref_stride8 += stride;
        ref_stride9 += stride;
    }
#else
    if((uint32_t)ref & 3)
    {
        uint8_t *ref_stride = ref + stride;
        uint8_t *ref_stride8 = ref + (stride << 3);
        uint8_t *ref_stride9 = ref_stride8 + stride;

        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref_stride[0] + rounding) >> 1;
            dst[1] = (ref[1] + ref_stride[1] + rounding) >> 1;
            dst[2] = (ref[2] + ref_stride[2] + rounding) >> 1;
            dst[3] = (ref[3] + ref_stride[3] + rounding) >> 1;
            dst[4] = (ref[4] + ref_stride[4] + rounding) >> 1;
            dst[5] = (ref[5] + ref_stride[5] + rounding) >> 1;
            dst[6] = (ref[6] + ref_stride[6] + rounding) >> 1;
            dst[7] = (ref[7] + ref_stride[7] + rounding) >> 1;

            dst[64] = (ref[8] + ref_stride[8] + rounding) >> 1;
            dst[65] = (ref[9] + ref_stride[9] + rounding) >> 1;
            dst[66] = (ref[10] + ref_stride[10] + rounding) >> 1;
            dst[67] = (ref[11] + ref_stride[11] + rounding) >> 1;
            dst[68] = (ref[12] + ref_stride[12] + rounding) >> 1;
            dst[69] = (ref[13] + ref_stride[13] + rounding) >> 1;
            dst[70] = (ref[14] + ref_stride[14] + rounding) >> 1;
            dst[71] = (ref[15] + ref_stride[15] + rounding) >> 1;

            dst[128] = (ref_stride8[0] + ref_stride9[0] + rounding) >> 1;
            dst[129] = (ref_stride8[1] + ref_stride9[1] + rounding) >> 1;
            dst[130] = (ref_stride8[2] + ref_stride9[2] + rounding) >> 1;
            dst[131] = (ref_stride8[3] + ref_stride9[3] + rounding) >> 1;
            dst[132] = (ref_stride8[4] + ref_stride9[4] + rounding) >> 1;
            dst[133] = (ref_stride8[5] + ref_stride9[5] + rounding) >> 1;
            dst[134] = (ref_stride8[6] + ref_stride9[6] + rounding) >> 1;
            dst[135] = (ref_stride8[7] + ref_stride9[7] + rounding) >> 1;

            dst[192] = (ref_stride8[8] + ref_stride9[8] + rounding) >> 1;
            dst[193] = (ref_stride8[9] + ref_stride9[9] + rounding) >> 1;
            dst[194] = (ref_stride8[10] + ref_stride9[10] + rounding) >> 1;
            dst[195] = (ref_stride8[11] + ref_stride9[11] + rounding) >> 1;
            dst[196] = (ref_stride8[12] + ref_stride9[12] + rounding) >> 1;
            dst[197] = (ref_stride8[13] + ref_stride9[13] + rounding) >> 1;
            dst[198] = (ref_stride8[14] + ref_stride9[14] + rounding) >> 1;
            dst[199] = (ref_stride8[15] + ref_stride9[15] + rounding) >> 1;

            dst += 8;
            ref += stride;
            ref_stride += stride;
            ref_stride8 += stride;
            ref_stride9 += stride;
        }
    }
    else
    {
#if 0
        uint32_t stride_tmp = stride >> 2;
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + stride);
        uint32_t *ref_addr_tmp2 = ref_addr_tmp0 + (stride << 1);
        uint32_t *ref_addr_tmp3 = ref_addr_tmp2 + stride_tmp;
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8);
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        long long refConst1=0xfefefeff;
        long long refConst2=0x01010100;

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[16] = (uint32_t)(((ref_addr_tmp0[2] & 0x1fefefeff)+((ref_addr_tmp1[2]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[17] = (uint32_t)(((ref_addr_tmp0[3] & 0x1fefefeff)+((ref_addr_tmp1[3]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp[32] = (uint32_t)(((ref_addr_tmp2[0] & 0x1fefefeff)+((ref_addr_tmp3[0]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[33] = (uint32_t)(((ref_addr_tmp2[1] & 0x1fefefeff)+((ref_addr_tmp3[1]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[48] = (uint32_t)(((ref_addr_tmp2[2] & 0x1fefefeff)+((ref_addr_tmp3[2]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[49] = (uint32_t)(((ref_addr_tmp2[3] & 0x1fefefeff)+((ref_addr_tmp3[3]
                                           + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
            ref_addr_tmp2 += stride_tmp;
            ref_addr_tmp3 += stride_tmp;
        }
#else
        /*
        uint32_t stride_tmp = stride >> 2;
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + stride);
        uint32_t *ref_addr_tmp2 = ref_addr_tmp0 + (stride << 1);
        uint32_t *ref_addr_tmp3 = ref_addr_tmp2 + stride_tmp;
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8);
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        for(i = 8;i > 0;i--)
        {
            dst_addr_tmp[0] = (uint32_t)((((ref_addr_tmp0[0] & 0x1fefefef)+(ref_addr_tmp1[0] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp0[0] & 0x01010100)+(ref_addr_tmp1[0] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[1] = (uint32_t)((((ref_addr_tmp0[1] & 0x1fefefef)+(ref_addr_tmp1[1] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp0[1] & 0x01010100)+(ref_addr_tmp1[1] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[16] = (uint32_t)((((ref_addr_tmp0[2] & 0x1fefefef)+(ref_addr_tmp1[2] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp0[2] & 0x01010100)+(ref_addr_tmp1[2] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[17] = (uint32_t)((((ref_addr_tmp0[3] & 0x1fefefef)+(ref_addr_tmp1[3] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp0[3] & 0x01010100)+(ref_addr_tmp1[3] & 0x01010100)+rounding_tmp)>>1)&0x01010100));

            dst_addr_tmp[32] = (uint32_t)((((ref_addr_tmp2[0] & 0x1fefefef)+(ref_addr_tmp3[0] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp2[0] & 0x01010100)+(ref_addr_tmp3[0] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[33] = (uint32_t)((((ref_addr_tmp2[1] & 0x1fefefef)+(ref_addr_tmp3[1] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp2[1] & 0x01010100)+(ref_addr_tmp3[1] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[48] = (uint32_t)((((ref_addr_tmp2[2] & 0x1fefefef)+(ref_addr_tmp3[2] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp2[2] & 0x01010100)+(ref_addr_tmp3[2] & 0x01010100)+rounding_tmp)>>1)&0x01010100));
            dst_addr_tmp[49] = (uint32_t)((((ref_addr_tmp2[3] & 0x1fefefef)+(ref_addr_tmp3[3] & 0x1fefefef) + rounding) >> 1) +
                ((((ref_addr_tmp2[3] & 0x01010100)+(ref_addr_tmp3[3] & 0x01010100)+rounding_tmp)>>1)&0x01010100));

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
            ref_addr_tmp2 += stride_tmp;
            ref_addr_tmp3 += stride_tmp;
        }
        */
        uint32_t stride_tmp = stride >> 2;
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + stride);
        uint32_t *ref_addr_tmp2 = ref_addr_tmp0 + (stride << 1);
        uint32_t *ref_addr_tmp3 = ref_addr_tmp2 + stride_tmp;
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;

        //0x1fefefeff +0x01010100=0x1ffff ffff
        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)((((ref_addr_tmp0[0] & 0xfefefeff)>>1)+((ref_addr_tmp1[0] & 0xfefefeff)>>1 ) ) +
                                         ((((ref_addr_tmp0[0] & 0x01010101)+(ref_addr_tmp1[0] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[1] = (uint32_t)((((ref_addr_tmp0[1] & 0xfefefeff)>>1)+((ref_addr_tmp1[1] & 0xfefefeff)>>1 ) ) +
                                         ((((ref_addr_tmp0[1] & 0x01010101)+(ref_addr_tmp1[1] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[16] = (uint32_t)((((ref_addr_tmp0[2] & 0xfefefeff)>>1)+((ref_addr_tmp1[2] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp0[2] & 0x01010101)+(ref_addr_tmp1[2] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[17] = (uint32_t)((((ref_addr_tmp0[3] & 0xfefefeff)>>1)+((ref_addr_tmp1[3] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp0[3] & 0x01010101)+(ref_addr_tmp1[3] & 0x01010101)+rounding_tmp)>>1)&0x01010101));

            dst_addr_tmp[32] = (uint32_t)((((ref_addr_tmp2[0] & 0xfefefeff)>>1)+((ref_addr_tmp3[0] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp2[0] & 0x01010101)+(ref_addr_tmp3[0] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[33] = (uint32_t)((((ref_addr_tmp2[1] & 0xfefefeff)>>1)+((ref_addr_tmp3[1] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp2[1] & 0x01010101)+(ref_addr_tmp3[1] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[48] = (uint32_t)((((ref_addr_tmp2[2] & 0xfefefeff)>>1)+((ref_addr_tmp3[2] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp2[2] & 0x01010101)+(ref_addr_tmp3[2] & 0x01010101)+rounding_tmp)>>1)&0x01010101));
            dst_addr_tmp[49] = (uint32_t)((((ref_addr_tmp2[3] & 0xfefefeff)>>1)+((ref_addr_tmp3[3] & 0xfefefeff)>>1 ) ) +
                                          ((((ref_addr_tmp2[3] & 0x01010101)+(ref_addr_tmp3[3] & 0x01010101)+rounding_tmp)>>1)&0x01010101));

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
            ref_addr_tmp2 += stride_tmp;
            ref_addr_tmp3 += stride_tmp;
        }

#endif
    }
#endif
}

void
interpolate16x16_halfpel_h_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    uint8_t *ref_stride = ref + (stride<<3);
    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref[1] + rounding) >> 1;
        dst[1] = (ref[1] + ref[2] + rounding) >> 1;
        dst[2] = (ref[2] + ref[3] + rounding) >> 1;
        dst[3] = (ref[3] + ref[4] + rounding) >> 1;

        dst[4] = (ref[4] + ref[5] + rounding) >> 1;
        dst[5] = (ref[5] + ref[6] + rounding) >> 1;
        dst[6] = (ref[6] + ref[7] + rounding) >> 1;
        dst[7] = (ref[7] + ref[8] + rounding) >> 1;

        dst[64] = (ref[8] + ref[9] + rounding) >> 1;
        dst[65] = (ref[9] + ref[10] + rounding) >> 1;
        dst[66] = (ref[10] + ref[11] + rounding) >> 1;
        dst[67] = (ref[11] + ref[12] + rounding) >> 1;

        dst[68] = (ref[12] + ref[13] + rounding) >> 1;
        dst[69] = (ref[13] + ref[14] + rounding) >> 1;
        dst[70] = (ref[14] + ref[15] + rounding) >> 1;
        dst[71] = (ref[15] + ref[16] + rounding) >> 1;

        dst[128] = (ref_stride[0] + ref_stride[1] + rounding) >> 1;
        dst[129] = (ref_stride[1] + ref_stride[2] + rounding) >> 1;
        dst[130] = (ref_stride[2] + ref_stride[3] + rounding) >> 1;
        dst[131] = (ref_stride[3] + ref_stride[4] + rounding) >> 1;

        dst[132] = (ref_stride[4] + ref_stride[5] + rounding) >> 1;
        dst[133] = (ref_stride[5] + ref_stride[6] + rounding) >> 1;
        dst[134] = (ref_stride[6] + ref_stride[7] + rounding) >> 1;
        dst[135] = (ref_stride[7] + ref_stride[8] + rounding) >> 1;

        dst[192] = (ref_stride[8] + ref_stride[9] + rounding) >> 1;
        dst[193] = (ref_stride[9] + ref_stride[10] + rounding) >> 1;
        dst[194] = (ref_stride[10] + ref_stride[11] + rounding) >> 1;
        dst[195] = (ref_stride[11] + ref_stride[12] + rounding) >> 1;

        dst[196] = (ref_stride[12] + ref_stride[13] + rounding) >> 1;
        dst[197] = (ref_stride[13] + ref_stride[14] + rounding) >> 1;
        dst[198] = (ref_stride[14] + ref_stride[15] + rounding) >> 1;
        dst[199] = (ref_stride[15] + ref_stride[16] + rounding) >> 1;

        dst += 8;
        ref += stride;
        ref_stride += stride;
    }
#else
    if((uint32_t)ref & 3)
    {
        uint8_t *ref_stride8 = ref + (stride<<3);
        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref[1] + rounding) >> 1;
            dst[1] = (ref[1] + ref[2] + rounding) >> 1;
            dst[2] = (ref[2] + ref[3] + rounding) >> 1;
            dst[3] = (ref[3] + ref[4] + rounding) >> 1;
            dst[4] = (ref[4] + ref[5] + rounding) >> 1;
            dst[5] = (ref[5] + ref[6] + rounding) >> 1;
            dst[6] = (ref[6] + ref[7] + rounding) >> 1;
            dst[7] = (ref[7] + ref[8] + rounding) >> 1;

            dst[64] = (ref[8] + ref[9] + rounding) >> 1;
            dst[65] = (ref[9] + ref[10] + rounding) >> 1;
            dst[66] = (ref[10] + ref[11] + rounding) >> 1;
            dst[67] = (ref[11] + ref[12] + rounding) >> 1;
            dst[68] = (ref[12] + ref[13] + rounding) >> 1;
            dst[69] = (ref[13] + ref[14] + rounding) >> 1;
            dst[70] = (ref[14] + ref[15] + rounding) >> 1;
            dst[71] = (ref[15] + ref[16] + rounding) >> 1;

            dst[128] = (ref_stride8[0] + ref_stride8[1] + rounding) >> 1;
            dst[129] = (ref_stride8[1] + ref_stride8[2] + rounding) >> 1;
            dst[130] = (ref_stride8[2] + ref_stride8[3] + rounding) >> 1;
            dst[131] = (ref_stride8[3] + ref_stride8[4] + rounding) >> 1;
            dst[132] = (ref_stride8[4] + ref_stride8[5] + rounding) >> 1;
            dst[133] = (ref_stride8[5] + ref_stride8[6] + rounding) >> 1;
            dst[134] = (ref_stride8[6] + ref_stride8[7] + rounding) >> 1;
            dst[135] = (ref_stride8[7] + ref_stride8[8] + rounding) >> 1;

            dst[192] = (ref_stride8[8] + ref_stride8[9] + rounding) >> 1;
            dst[193] = (ref_stride8[9] + ref_stride8[10] + rounding) >> 1;
            dst[194] = (ref_stride8[10] + ref_stride8[11] + rounding) >> 1;
            dst[195] = (ref_stride8[11] + ref_stride8[12] + rounding) >> 1;
            dst[196] = (ref_stride8[12] + ref_stride8[13] + rounding) >> 1;
            dst[197] = (ref_stride8[13] + ref_stride8[14] + rounding) >> 1;
            dst[198] = (ref_stride8[14] + ref_stride8[15] + rounding) >> 1;
            dst[199] = (ref_stride8[15] + ref_stride8[16] + rounding) >> 1;

            dst += 8;
            ref += stride;
            ref_stride8 += stride;
        }
    }
    else
    {
        uint32_t stride_tmp = stride >> 2;
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + 1);
        uint32_t *ref_addr_tmp2 = (uint32_t *)(ref +(stride << 3));
        uint32_t *ref_addr_tmp3 = (uint32_t *)(ref_addr_tmp1 + (stride_tmp << 3));
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp[16] = (uint32_t)(((ref_addr_tmp0[2] & 0x1fefefeff)+((ref_addr_tmp1[2] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[17] = (uint32_t)(((ref_addr_tmp0[3] & 0x1fefefeff)+((ref_addr_tmp1[3] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp[32] = (uint32_t)(((ref_addr_tmp2[0] & 0x1fefefeff)+((ref_addr_tmp3[0] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[33] = (uint32_t)(((ref_addr_tmp2[1] & 0x1fefefeff)+((ref_addr_tmp3[1] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp[48] = (uint32_t)(((ref_addr_tmp2[2] & 0x1fefefeff)+((ref_addr_tmp3[2] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[49] = (uint32_t)(((ref_addr_tmp2[3] & 0x1fefefeff)+((ref_addr_tmp3[3] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
            ref_addr_tmp2 += stride_tmp;
            ref_addr_tmp3 += stride_tmp;
        }
    }
#endif
}

void
interpolate16x16_halfpel_hv_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    uint8_t *ref_stride = ref + stride;
    uint8_t *ref_stride8 = ref + (stride << 3);
    uint8_t *ref_stride9 = ref_stride8 + stride;

    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref[1] + ref_stride[0] + ref_stride[1] + rounding) >> 2;
        dst[1] = (ref[1] + ref[2] + ref_stride[1] + ref_stride[2] + rounding) >> 2;
        dst[2] = (ref[2] + ref[3] + ref_stride[2] + ref_stride[3] + rounding) >> 2;
        dst[3] = (ref[3] + ref[4] + ref_stride[3] + ref_stride[4] + rounding) >> 2;

        dst[4] = (ref[4] + ref[5] + ref_stride[4] + ref_stride[5] + rounding) >> 2;
        dst[5] = (ref[5] + ref[6] + ref_stride[5] + ref_stride[6] + rounding) >> 2;
        dst[6] = (ref[6] + ref[7] + ref_stride[6] + ref_stride[7] + rounding) >> 2;
        dst[7] = (ref[7] + ref[8] + ref_stride[7] + ref_stride[8] + rounding) >> 2;

        dst[64] = (ref[8] + ref[9] + ref_stride[8] + ref_stride[9] + rounding) >> 2;
        dst[65] = (ref[9] + ref[10] + ref_stride[9] + ref_stride[10] + rounding) >> 2;
        dst[66] = (ref[10] + ref[11] + ref_stride[10] + ref_stride[11] + rounding) >> 2;
        dst[67] = (ref[11] + ref[12] + ref_stride[11] + ref_stride[12] + rounding) >> 2;

        dst[68] = (ref[12] + ref[13] + ref_stride[12] + ref_stride[13] + rounding) >> 2;
        dst[69] = (ref[13] + ref[14] + ref_stride[13] + ref_stride[14] + rounding) >> 2;
        dst[70] = (ref[14] + ref[15] + ref_stride[14] + ref_stride[15] + rounding) >> 2;
        dst[71] = (ref[15] + ref[16] + ref_stride[15] + ref_stride[16] + rounding) >> 2;

        dst[128] = (ref_stride8[0] + ref_stride8[1] + ref_stride9[0] + ref_stride9[1] + rounding) >> 2;
        dst[129] = (ref_stride8[1] + ref_stride8[2] + ref_stride9[1] + ref_stride9[2] + rounding) >> 2;
        dst[130] = (ref_stride8[2] + ref_stride8[3] + ref_stride9[2] + ref_stride9[3] + rounding) >> 2;
        dst[131] = (ref_stride8[3] + ref_stride8[4] + ref_stride9[3] + ref_stride9[4] + rounding) >> 2;

        dst[132] = (ref_stride8[4] + ref_stride8[5] + ref_stride9[4] + ref_stride9[5] + rounding) >> 2;
        dst[133] = (ref_stride8[5] + ref_stride8[6] + ref_stride9[5] + ref_stride9[6] + rounding) >> 2;
        dst[134] = (ref_stride8[6] + ref_stride8[7] + ref_stride9[6] + ref_stride9[7] + rounding) >> 2;
        dst[135] = (ref_stride8[7] + ref_stride8[8] + ref_stride9[7] + ref_stride9[8] + rounding) >> 2;

        dst[192] = (ref_stride8[8] + ref_stride8[9] + ref_stride9[8] + ref_stride9[9] + rounding) >> 2;
        dst[193] = (ref_stride8[9] + ref_stride8[10] + ref_stride9[9] + ref_stride9[10] + rounding) >> 2;
        dst[194] = (ref_stride8[10] + ref_stride8[11] + ref_stride9[10] + ref_stride9[11] + rounding) >> 2;
        dst[195] = (ref_stride8[11] + ref_stride8[12] + ref_stride9[11] + ref_stride9[12] + rounding) >> 2;

        dst[196] = (ref_stride8[12] + ref_stride8[13] + ref_stride9[12] + ref_stride9[13] + rounding) >> 2;
        dst[197] = (ref_stride8[13] + ref_stride8[14] + ref_stride9[13] + ref_stride9[14] + rounding) >> 2;
        dst[198] = (ref_stride8[14] + ref_stride8[15] + ref_stride9[14] + ref_stride9[15] + rounding) >> 2;
        dst[199] = (ref_stride8[15] + ref_stride8[16] + ref_stride9[15] + ref_stride9[16] + rounding) >> 2;

        dst += 8;
        ref += stride;
        ref_stride += stride;
        ref_stride8 += stride;
        ref_stride9 += stride;
    }
#else
#if 1
    uint8_t *ref_stride = ref + stride;

    for(i = 16; i > 0; i--)
    {
        dst[0] = (ref[0] + ref[1] + ref_stride[0] + ref_stride[1] + rounding) >> 2;
        dst[1] = (ref[1] + ref[2] + ref_stride[1] + ref_stride[2] + rounding) >> 2;
        dst[2] = (ref[2] + ref[3] + ref_stride[2] + ref_stride[3] + rounding) >> 2;
        dst[3] = (ref[3] + ref[4] + ref_stride[3] + ref_stride[4] + rounding) >> 2;

        dst[4] = (ref[4] + ref[5] + ref_stride[4] + ref_stride[5] + rounding) >> 2;
        dst[5] = (ref[5] + ref[6] + ref_stride[5] + ref_stride[6] + rounding) >> 2;
        dst[6] = (ref[6] + ref[7] + ref_stride[6] + ref_stride[7] + rounding) >> 2;
        dst[7] = (ref[7] + ref[8] + ref_stride[7] + ref_stride[8] + rounding) >> 2;

        dst[8] = (ref[8] + ref[9] + ref_stride[8] + ref_stride[9] + rounding) >> 2;
        dst[9] = (ref[9] + ref[10] + ref_stride[9] + ref_stride[10] + rounding) >> 2;
        dst[10] = (ref[10] + ref[11] + ref_stride[10] + ref_stride[11] + rounding) >> 2;
        dst[11] = (ref[11] + ref[12] + ref_stride[11] + ref_stride[12] + rounding) >> 2;

        dst[12] = (ref[12] + ref[13] + ref_stride[12] + ref_stride[13] + rounding) >> 2;
        dst[13] = (ref[13] + ref[14] + ref_stride[13] + ref_stride[14] + rounding) >> 2;
        dst[14] = (ref[14] + ref[15] + ref_stride[14] + ref_stride[15] + rounding) >> 2;
        dst[15] = (ref[15] + ref[16] + ref_stride[15] + ref_stride[16] + rounding) >> 2;

        dst += 16;
        ref += stride;
        ref_stride += stride;
    }
#else
    uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
    uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + 1);
    uint32_t *ref_addr_tmp2 = (uint32_t *)(ref + stride);
    uint32_t *ref_addr_tmp3 = (uint32_t *)(ref + stride + 1);
    uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
    uint32_t *dst_addr_tmp = (uint32_t *)dst;
    uint32_t stride_tmp = (stride >> 2) - 4;

    for (i = 16; i > 0; i--)
    {
        *dst_addr_tmp++ = ((((((*ref_addr_tmp0++) & 0x1fefefeff) + ((*ref_addr_tmp1++) & 0x1fefefeff))>>1) & 0x1fefefeff)
                           + (((((*ref_addr_tmp2++) & 0x1fefefeff) + ((*(ref_addr_tmp3++) + rounding_tmp) & 0x1fefefeff))>>1) & 0x1fefefeff)) >> 1;
        *dst_addr_tmp++ = ((((((*ref_addr_tmp0++) & 0x1fefefeff) + ((*ref_addr_tmp1++) & 0x1fefefeff))>>1) & 0x1fefefeff)
                           + (((((*ref_addr_tmp2++) & 0x1fefefeff) + ((*(ref_addr_tmp3++) + rounding_tmp) & 0x1fefefeff))>>1) & 0x1fefefeff)) >> 1;
        *dst_addr_tmp++ = ((((((*ref_addr_tmp0++) & 0x1fefefeff) + ((*ref_addr_tmp1++) & 0x1fefefeff))>>1) & 0x1fefefeff)
                           + (((((*ref_addr_tmp2++) & 0x1fefefeff) + ((*(ref_addr_tmp3++) + rounding_tmp) & 0x1fefefeff))>>1) & 0x1fefefeff)) >> 1;
        *dst_addr_tmp++ = ((((((*ref_addr_tmp0++) & 0x1fefefeff) + ((*ref_addr_tmp1++) & 0x1fefefeff))>>1) & 0x1fefefeff)
                           + (((((*ref_addr_tmp2++) & 0x1fefefeff) + ((*(ref_addr_tmp3++) + rounding_tmp) & 0x1fefefeff))>>1) & 0x1fefefeff)) >> 1;

        dst_addr_tmp += 4;
        ref_addr_tmp0 += stride_tmp;
        ref_addr_tmp1 += stride_tmp;
        ref_addr_tmp2 += stride_tmp;
        ref_addr_tmp3 += stride_tmp;
    }
#endif
#endif
}

void
mp4_transfer8x8_copy_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
    uint8_t ref_diff = (uint32_t)ref & 3;

    switch(ref_diff)
    {
        case 0:
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
            uint32_t *dst_addr_tmp = (uint32_t *)dst;
            uint32_t stride_tmp = (stride >> 2);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = ref_addr_tmp0[0];
                dst_addr_tmp[1] = ref_addr_tmp0[1];

                dst_addr_tmp += 2;
                ref_addr_tmp0 += stride_tmp;
            }
        }
        break;
        case 2:
        {
            uint16_t *ref_16 = (uint16_t *)ref;
            uint16_t *dst_16 = (uint16_t *)dst;
            int32_t stride_tmp = stride >> 1;
            for(i = 8; i > 0; i--)
            {
                dst_16[0] = ref_16[0];
                dst_16[1] = ref_16[1];
                dst_16[2] = ref_16[2];
                dst_16[3] = ref_16[3];

                dst_16 += 4;
                ref_16 += stride_tmp;
            }
        }
        break;
        default:
            for(i = 8; i > 0; i--)
            {
                dst[0] = ref[0];
                dst[1] = ref[1];
                dst[2] = ref[2];
                dst[3] = ref[3];

                dst[4] = ref[4];
                dst[5] = ref[5];
                dst[6] = ref[6];
                dst[7] = ref[7];

                dst += 8;
                ref += stride;
            }
            break;
    }
}

void
mp4_interpolate8x8_halfpel_v_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    uint8_t *ref_stride = ref + stride;
    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref_stride[0] + rounding) >> 1;
        dst[1] = (ref[1] + ref_stride[1] + rounding) >> 1;
        dst[2] = (ref[2] + ref_stride[2] + rounding) >> 1;
        dst[3] = (ref[3] + ref_stride[3] + rounding) >> 1;

        dst[4] = (ref[4] + ref_stride[4] + rounding) >> 1;
        dst[5] = (ref[5] + ref_stride[5] + rounding) >> 1;
        dst[6] = (ref[6] + ref_stride[6] + rounding) >> 1;
        dst[7] = (ref[7] + ref_stride[7] + rounding) >> 1;

        dst += 8;
        ref += stride;
        ref_stride += stride;
    }
#else
    if((uint32_t)ref & 3)
    {
        uint8_t *ref_stride = ref + stride;

        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref_stride[0] + rounding) >> 1;
            dst[1] = (ref[1] + ref_stride[1] + rounding) >> 1;
            dst[2] = (ref[2] + ref_stride[2] + rounding) >> 1;
            dst[3] = (ref[3] + ref_stride[3] + rounding) >> 1;

            dst[4] = (ref[4] + ref_stride[4] + rounding) >> 1;
            dst[5] = (ref[5] + ref_stride[5] + rounding) >> 1;
            dst[6] = (ref[6] + ref_stride[6] + rounding) >> 1;
            dst[7] = (ref[7] + ref_stride[7] + rounding) >> 1;

            dst += 8;
            ref += stride;
            ref_stride += stride;
        }
    }
    else
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + stride);
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        uint32_t stride_tmp = stride >> 2;

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
        }
    }
#endif
}

void
mp4_interpolate8x8_halfpel_h_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref[1] + rounding) >> 1;
        dst[1] = (ref[1] + ref[2] + rounding) >> 1;
        dst[2] = (ref[2] + ref[3] + rounding) >> 1;
        dst[3] = (ref[3] + ref[4] + rounding) >> 1;

        dst[4] = (ref[4] + ref[5] + rounding) >> 1;
        dst[5] = (ref[5] + ref[6] + rounding) >> 1;
        dst[6] = (ref[6] + ref[7] + rounding) >> 1;
        dst[7] = (ref[7] + ref[8] + rounding) >> 1;

        dst += 8;
        ref += stride;
    }
#else
    if((uint32_t)ref & 3)
    {
        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref[1] + rounding) >> 1;
            dst[1] = (ref[1] + ref[2] + rounding) >> 1;
            dst[2] = (ref[2] + ref[3] + rounding) >> 1;
            dst[3] = (ref[3] + ref[4] + rounding) >> 1;

            dst[4] = (ref[4] + ref[5] + rounding) >> 1;
            dst[5] = (ref[5] + ref[6] + rounding) >> 1;
            dst[6] = (ref[6] + ref[7] + rounding) >> 1;
            dst[7] = (ref[7] + ref[8] + rounding) >> 1;

            dst += 8;
            ref += stride;
        }
    }
    else
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + 1);
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        uint32_t stride_tmp = stride >> 2;
        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
        }
    }
#endif
}

void
mp4_interpolate8x8_halfpel_hv_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
    uint8_t *ref_strdie = ref + stride;

    for(i = 8; i > 0; i--)
    {
        dst[0] = (ref[0] + ref[1] + ref_strdie[0] + ref_strdie[1] + rounding) >> 2;
        dst[1] = (ref[1] + ref[2] + ref_strdie[1] + ref_strdie[2] + rounding) >> 2;
        dst[2] = (ref[2] + ref[3] + ref_strdie[2] + ref_strdie[3] + rounding) >> 2;
        dst[3] = (ref[3] + ref[4] + ref_strdie[3] + ref_strdie[4] +  rounding) >> 2;

        dst[4] = (ref[4] + ref[5] + ref_strdie[4] + ref_strdie[5] + rounding) >> 2;
        dst[5] = (ref[5] + ref[6] + ref_strdie[5] + ref_strdie[6] + rounding) >> 2;
        dst[6] = (ref[6] + ref[7] + ref_strdie[6] + ref_strdie[7] + rounding) >> 2;
        dst[7] = (ref[7] + ref[8] + ref_strdie[7] + ref_strdie[8] + rounding) >> 2;

        dst += 8;
        ref += stride;
        ref_strdie += stride;
    }
}
void
mp4_transfer8x8_add_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
    uint8_t ref_diff = (uint32_t)ref & 3;
    ref_diff = 0xff;//by yongheng
    switch(ref_diff)
    {
        case 0:
        {
            uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
            uint32_t *dst_addr_tmp = (uint32_t *)dst;
            uint32_t stride_tmp = (stride >> 2);

            for(i = 8; i > 0; i--)
            {
                dst_addr_tmp[0] = (dst_addr_tmp[0] + ref_addr_tmp0[0] + rounding)>>1;
                dst_addr_tmp[1] = (dst_addr_tmp[1] + ref_addr_tmp0[1] + rounding)>>1;

                dst_addr_tmp += 2;
                ref_addr_tmp0 += stride_tmp;
            }
        }
        break;
        case 2:
        {
            uint16_t *ref_16 = (uint16_t *)ref;
            uint16_t *dst_16 = (uint16_t *)dst;
            int32_t stride_tmp = stride >> 1;
            for(i = 8; i > 0; i--)
            {
                dst_16[0] = (dst_16[0] + ref_16[0] + rounding)>>1;
                dst_16[1] = (dst_16[1] + ref_16[1] + rounding)>>1;
                dst_16[2] = (dst_16[2] + ref_16[2] + rounding)>>1;;
                dst_16[3] = (dst_16[3] + ref_16[3] + rounding)>>1;

                dst_16 += 4;
                ref_16 += stride_tmp;
            }
        }
        break;
        default:
            for(i = 8; i > 0; i--)
            {
                dst[0] = (dst[0] + ref[0] + rounding)>>1;
                dst[1] = (dst[1] + ref[1] + rounding)>>1;
                dst[2] = (dst[2] + ref[2] + rounding)>>1;
                dst[3] = (dst[3] + ref[3] + rounding)>>1;

                dst[4] = (dst[4] + ref[4] + rounding)>>1;
                dst[5] = (dst[5] + ref[5] + rounding)>>1;
                dst[6] = (dst[6] + ref[6] + rounding)>>1;
                dst[7] = (dst[7] + ref[7] + rounding)>>1;

                dst += 8;
                ref += stride;
            }
            break;
    }
}

void
mp4_interpolate8x8_halfpel_v_add_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    uint8_t *ref_stride = ref + stride;

    for(i = 8; i > 0; i--)
    {
        dst[0] = (((ref[0] + ref_stride[0] + rounding) >> 1) + dst[0] + 1)>>1;
        dst[1] = (((ref[1] + ref_stride[1] + rounding) >> 1) + dst[1] + 1)>>1;
        dst[2] = (((ref[2] + ref_stride[2] + rounding) >> 1) + dst[2] + 1)>>1;
        dst[3] = (((ref[3] + ref_stride[3] + rounding) >> 1) + dst[3] + 1)>>1;

        dst[4] = (((ref[4] + ref_stride[4] + rounding) >> 1) + dst[4] + 1)>>1;
        dst[5] = (((ref[5] + ref_stride[5] + rounding) >> 1) + dst[5] + 1)>>1;
        dst[6] = (((ref[6] + ref_stride[6] + rounding) >> 1) + dst[6] + 1)>>1;
        dst[7] = (((ref[7] + ref_stride[7] + rounding) >> 1) + dst[7] + 1)>>1;


        dst += 8;
        ref += stride;
        ref_stride += stride;
    }

#else
    if((uint32_t)ref & 3)
    {
        uint8_t *ref_stride = ref + stride;

        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref_stride[0] + rounding) >> 1;
            dst[1] = (ref[1] + ref_stride[1] + rounding) >> 1;
            dst[2] = (ref[2] + ref_stride[2] + rounding) >> 1;
            dst[3] = (ref[3] + ref_stride[3] + rounding) >> 1;

            dst[4] = (ref[4] + ref_stride[4] + rounding) >> 1;
            dst[5] = (ref[5] + ref_stride[5] + rounding) >> 1;
            dst[6] = (ref[6] + ref_stride[6] + rounding) >> 1;
            dst[7] = (ref[7] + ref_stride[7] + rounding) >> 1;

            dst += 8;
            ref += stride;
            ref_stride += stride;
        }
    }
    else
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + stride);
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        uint32_t stride_tmp = stride >> 2;

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1]
                                          + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
        }
    }
#endif
}

void
mp4_interpolate8x8_halfpel_h_add_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
#if 1
    for(i = 8; i > 0; i--)
    {
        dst[0] = (((ref[0] + ref[1] + rounding) >> 1) + dst[0] + 1)>>1;
        dst[1] = (((ref[1] + ref[2] + rounding) >> 1) + dst[1] + 1)>>1;
        dst[2] = (((ref[2] + ref[3] + rounding) >> 1) + dst[2] + 1)>>1;
        dst[3] = (((ref[3] + ref[4] + rounding) >> 1) + dst[3] + 1)>>1;

        dst[4] = (((ref[4] + ref[5] + rounding) >> 1) + dst[4] + 1)>>1;
        dst[5] = (((ref[5] + ref[6] + rounding) >> 1) + dst[5] + 1)>>1;
        dst[6] = (((ref[6] + ref[7] + rounding) >> 1) + dst[6] + 1)>>1;
        dst[7] = (((ref[7] + ref[8] + rounding) >> 1) + dst[7] + 1)>>1;


        dst += 8;
        ref += stride;
    }
#else
    if((uint32_t)ref & 3)
    {
        for(i = 8; i > 0; i--)
        {
            dst[0] = (ref[0] + ref[1] + rounding) >> 1;
            dst[1] = (ref[1] + ref[2] + rounding) >> 1;
            dst[2] = (ref[2] + ref[3] + rounding) >> 1;
            dst[3] = (ref[3] + ref[4] + rounding) >> 1;

            dst[4] = (ref[4] + ref[5] + rounding) >> 1;
            dst[5] = (ref[5] + ref[6] + rounding) >> 1;
            dst[6] = (ref[6] + ref[7] + rounding) >> 1;
            dst[7] = (ref[7] + ref[8] + rounding) >> 1;

            dst += 8;
            ref += stride;
        }
    }
    else
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)ref;
        uint32_t *ref_addr_tmp1 = (uint32_t *)(ref + 1);
        uint32_t rounding_tmp = (rounding<<24)|(rounding<<16)|(rounding<<8)|rounding;
        uint32_t *dst_addr_tmp = (uint32_t *)dst;
        uint32_t stride_tmp = stride >> 2;
        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = (uint32_t)(((ref_addr_tmp0[0] & 0x1fefefeff)+((ref_addr_tmp1[0] + rounding_tmp) & 0x1fefefeff)) >> 1);
            dst_addr_tmp[1] = (uint32_t)(((ref_addr_tmp0[1] & 0x1fefefeff)+((ref_addr_tmp1[1] + rounding_tmp) & 0x1fefefeff)) >> 1);

            dst_addr_tmp += 2;
            ref_addr_tmp0 += stride_tmp;
            ref_addr_tmp1 += stride_tmp;
        }
    }
#endif
}

void
mp4_interpolate8x8_halfpel_hv_add_c(uint8_t *dst,uint8_t *ref,int32_t stride,int32_t rounding)
{
    int16_t i;
    uint8_t *ref_strdie = ref + stride;
    for(i = 8; i > 0; i--)
    {
        dst[0] = (((ref[0] + ref[1] + ref_strdie[0] + ref_strdie[1] + rounding) >> 2) + dst[0] + 1)>>1;
        dst[1] = (((ref[1] + ref[2] + ref_strdie[1] + ref_strdie[2] + rounding) >> 2) + dst[1] + 1)>>1;
        dst[2] = (((ref[2] + ref[3] + ref_strdie[2] + ref_strdie[3] + rounding) >> 2) + dst[2] + 1)>>1;
        dst[3] = (((ref[3] + ref[4] + ref_strdie[3] + ref_strdie[4] + rounding) >> 2) + dst[3] + 1)>>1;

        dst[4] = (((ref[4] + ref[5] + ref_strdie[4] + ref_strdie[5] + rounding) >> 2) + dst[4] + 1)>>1;
        dst[5] = (((ref[5] + ref[6] + ref_strdie[5] + ref_strdie[6] + rounding) >> 2) + dst[5] + 1)>>1;
        dst[6] = (((ref[6] + ref[7] + ref_strdie[6] + ref_strdie[7] + rounding) >> 2) + dst[6] + 1)>>1;
        dst[7] = (((ref[7] + ref[8] + ref_strdie[7] + ref_strdie[8] + rounding) >> 2) + dst[7] + 1)>>1;



        dst += 8;
        ref += stride;
        ref_strdie += stride;
    }
}

static uint32_t rnd_avg32(uint32_t a, uint32_t b)
{
    return (a | b) - (((a ^ b) & ~BYTE_VEC32(0x01)) >> 1);//#define  BYTE_VEC32(c)   ((c)*0x01010101UL)
}
static void mp4_chang_array8to16(uint8_t *dst, uint8_t *src,int dst_stride)
{
    int i;
    int add_temp = 8*dst_stride;
    for(i=0; i<8; i++ )
    {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];

        dst[8] = src[64];
        dst[9] = src[65];
        dst[10] = src[66];
        dst[11] = src[67];
        dst[12] = src[68];
        dst[13] = src[69];
        dst[14] = src[70];
        dst[15] = src[71];

        dst[0+add_temp] = src[128];
        dst[1+add_temp] = src[129];
        dst[2+add_temp] = src[130];
        dst[3+add_temp] = src[131];
        dst[4+add_temp] = src[132];
        dst[5+add_temp] = src[133];
        dst[6+add_temp] = src[134];
        dst[7+add_temp] = src[135];

        dst[0+add_temp] = src[192];
        dst[1+add_temp] = src[193];
        dst[2+add_temp] = src[194];
        dst[3+add_temp] = src[195];
        dst[4+add_temp] = src[196];
        dst[5+add_temp] = src[197];
        dst[6+add_temp] = src[198];
        dst[7+add_temp] = src[199];

        dst+=dst_stride;
        src+=8;
    }
}

void put_mp4_pixels8_l2_8(uint8_t *dst, const uint8_t *src1, const uint8_t *src2,
                          int dst_stride,int src_stride1,int src_stride2,int h)
{
    int i;
    for(i=0; i<h; i++)
    {
        //uint32_t a,b;
        //a= AV_RN32(&src1[i*src_stride1  ]);
        //b= AV_RN32(&src2[i*src_stride2  ]);
        //*((uint32_t*)&dst[i*dst_stride  ]) = rnd_avg32(a, b);
        //a= AV_RN32(&src1[i*src_stride1+4]);
        //b= AV_RN32(&src2[i*src_stride2+4]);
        //*((uint32_t*)&dst[i*dst_stride+4]) = rnd_avg32(a, b);
        dst[0] = (src1[0] + src2[0] + 1)>>1;
        dst[1] = (src1[1] + src2[1] + 1)>>1;
        dst[2] = (src1[2] + src2[2] + 1)>>1;
        dst[3] = (src1[3] + src2[3] + 1)>>1;
        dst[4] = (src1[4] + src2[4] + 1)>>1;
        dst[5] = (src1[5] + src2[5] + 1)>>1;
        dst[6] = (src1[6] + src2[6] + 1)>>1;
        dst[7] = (src1[7] + src2[7] + 1)>>1;

        dst+=dst_stride;
        src1+=src_stride1;
        src2+=src_stride2;
    }
}

void put_mp4_add_pixels8_l2_8(uint8_t *dst, const uint8_t *src1, const uint8_t *src2,
                              int dst_stride,int src_stride1,int src_stride2,int h)
{
    int i;
    uint8_t temp;
    for(i=0; i<h; i++)
    {
        //uint32_t a,b;
        //a= AV_RN32(&src1[i*src_stride1  ]);
        //b= AV_RN32(&src2[i*src_stride2  ]);
        //*((uint32_t*)&dst[i*dst_stride  ]) = rnd_avg32(a, b);
        //a= AV_RN32(&src1[i*src_stride1+4]);
        //b= AV_RN32(&src2[i*src_stride2+4]);
        //*((uint32_t*)&dst[i*dst_stride+4]) = rnd_avg32(a, b);
        temp = (src1[0] + src2[0] + 1)>>1;
        dst[0] = (dst[0] + temp+ 1)>>1;
        temp = (src1[1] + src2[1] + 1)>>1;
        dst[1] = (dst[1] + temp+ 1)>>1;
        temp = (src1[2] + src2[2] + 1)>>1;
        dst[2] = (dst[2] + temp+ 1)>>1;
        temp = (src1[3] + src2[3] + 1)>>1;
        dst[3] = (dst[3] + temp+ 1)>>1;
        temp = (src1[4] + src2[4] + 1)>>1;
        dst[4] = (dst[4] + temp+ 1)>>1;
        temp = (src1[5] + src2[5] + 1)>>1;
        dst[5] = (dst[5] + temp+ 1)>>1;
        temp = (src1[6] + src2[6] + 1)>>1;
        dst[6] = (dst[6] + temp+ 1)>>1;
        temp = (src1[7] + src2[7] + 1)>>1;
        dst[7] = (dst[7] + temp+ 1)>>1;

        dst+=dst_stride;
        src1+=src_stride1;
        src2+=src_stride2;
    }
}

void put_mp4_pixels16_l2_8(uint8_t *dst, const uint8_t *src1, const uint8_t *src2,
                           int src_stride1)
{
    int add_temp = 8*src_stride1;
    put_mp4_pixels8_l2_8(dst  , src1  , src2  , 8, src_stride1, 8, 8);
    put_mp4_pixels8_l2_8(dst+64, src1+8, src2+64, 8, src_stride1, 8, 8);
    put_mp4_pixels8_l2_8(dst+128, src1+add_temp, src2+128, 8, src_stride1, 8, 8);
    put_mp4_pixels8_l2_8(dst+192, src1+add_temp+8, src2+192, 8, src_stride1, 8, 8);
}

void put_mp4_pixels_16X16(uint8_t *dst, const uint8_t *src1, const uint8_t *src2,
                          int src_stride1,int h)
{
    put_mp4_pixels8_l2_8(dst  , src1  , src2  , 16, src_stride1, 16, h);
    put_mp4_pixels8_l2_8(dst+8  , src1+8  , src2+8  , 16, src_stride1, 16, h);
}

void mp4_qpel8_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
    int i;
    int src_temp;
    for(i=0; i<h; i++)
    {
        const int src0= src[0];
        const int src1= src[1];
        const int src2= src[2];
        const int src3= src[3];
        const int src4= src[4];
        const int src5= src[5];
        const int src6= src[6];
        const int src7= src[7];
        const int src8= src[8];
        //OP(dst[0], (src[0]+src[1])*20 - (src[0]+src[2])*6 + (src[1]+src[3])*3 - (src[2]+src[4]));
        src_temp = ((src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4) + 16)>>5;
        dst[0] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[1], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src[5]));
        src_temp = ((src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5) + 16)>>5;
        dst[1] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[2], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src[6]));
        src_temp = ((src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6) + 16)>>5;
        dst[2] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[3], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src[7]));
        src_temp = ((src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7) + 16)>>5;
        dst[3] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[4], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src[8]));
        src_temp = ((src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8) + 16)>>5;
        dst[4] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[5], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));
        src_temp = ((src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8) + 16)>>5;
        dst[5] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[6], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));
        src_temp = ((src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7) + 16)>>5;
        dst[6] = BYTE_LIMT(ABS(src_temp));
        //OP(dst[7], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));
        src_temp = ((src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6) + 16)>>5;
        dst[7] = BYTE_LIMT(ABS(src_temp));
        dst+=dstStride;
        src+=srcStride;
    }
}


void mp4_qpel8_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
    const int w=8;
    int i;
    for(i=0; i<w; i++)
    {
        const int src0= src[0*srcStride];
        const int src1= src[1*srcStride];
        const int src2= src[2*srcStride];
        const int src3= src[3*srcStride];
        const int src4= src[4*srcStride];
        const int src5= src[5*srcStride];
        const int src6= src[6*srcStride];
        const int src7= src[7*srcStride];
        const int src8= src[8*srcStride];

        //OP(dst[0*dstStride], (src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4));
        dst[0*dstStride] = BYTE_LIMT(ABS(((src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4) + 16)>>5));
        //OP(dst[1*dstStride], (src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5));
        dst[1*dstStride] = BYTE_LIMT(ABS(((src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5) + 16)>>5));
        //OP(dst[2*dstStride], (src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6));
        dst[2*dstStride] = BYTE_LIMT(ABS(((src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6) + 16)>>5));
        //OP(dst[3*dstStride], (src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7));
        dst[3*dstStride] = BYTE_LIMT(ABS(((src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7) + 16)>>5));
        //OP(dst[4*dstStride], (src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8));
        dst[4*dstStride] = BYTE_LIMT(ABS(((src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8) + 16)>>5));
        //OP(dst[5*dstStride], (src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8));
        dst[5*dstStride] = BYTE_LIMT(ABS(((src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src8) + 16)>>5));
        //OP(dst[6*dstStride], (src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7));
        dst[6*dstStride] = BYTE_LIMT(ABS(((src6+src7)*20 - (src5+src8)*6 + (src4+src8)*3 - (src3+src7) + 16)>>5));
        //OP(dst[7*dstStride], (src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6));
        dst[7*dstStride] = BYTE_LIMT(ABS(((src7+src8)*20 - (src6+src8)*6 + (src5+src7)*3 - (src4+src6) + 16)>>5));
        dst++;
        src++;
    }
}

void mp4_qpel16_h_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
    int i;
    int src_temp;
    uint8_t *ref_src = src + (srcStride<<3);
    for(i=0; i<h; i++)
    {
        const int src0= src[0];
        const int src1= src[1];
        const int src2= src[2];
        const int src3= src[3];
        const int src4= src[4];
        const int src5= src[5];
        const int src6= src[6];
        const int src7= src[7];
        const int src8= src[8];
        const int src9= src[9];
        const int src10= src[10];
        const int src11= src[11];
        const int src12= src[12];
        const int src13= src[13];
        const int src14= src[14];
        const int src15= src[15];
        const int src16= src[16];
        const int ref_src0= ref_src[0];
        const int ref_src1= ref_src[1];
        const int ref_src2= ref_src[2];
        const int ref_src3= ref_src[3];
        const int ref_src4= ref_src[4];
        const int ref_src5= ref_src[5];
        const int ref_src6= ref_src[6];
        const int ref_src7= ref_src[7];
        const int ref_src8= ref_src[8];
        const int ref_src9= ref_src[9];
        const int ref_src10= ref_src[10];
        const int ref_src11= ref_src[11];
        const int ref_src12= ref_src[12];
        const int ref_src13= ref_src[13];
        const int ref_src14= ref_src[14];
        const int ref_src15= ref_src[15];
        const int ref_src16= ref_src[16];
        src_temp = ((src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4) + 16)>>5;
        dst[0] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5) + 16)>>5;
        dst[1] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6) + 16)>>5;
        dst[2] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7) + 16)>>5;
        dst[3] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8) + 16)>>5;
        dst[4] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src9) + 16)>>5;
        dst[5] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src6+src7)*20 - (src5+src8)*6 + (src4+src9)*3 - (src3+src10) + 16)>>5;
        dst[6] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src7+src8)*20 - (src6+src9)*6 + (src5+src10)*3 - (src4+src11) + 16)>>5;
        dst[7] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src8+src9)*20 - (src7+src10)*6 + (src6+src11)*3 - (src5+src12) + 16)>>5;
        dst[64] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src9+src10)*20 - (src8+src11)*6 + (src7+src12)*3 - (src6+src13) + 16)>>5;
        dst[65] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src10+src11)*20 - (src9+src12)*6 + (src8+src13)*3 - (src7+src14) + 16)>>5;
        dst[66] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src11+src12)*20 - (src10+src13)*6 + (src9+src14)*3 - (src8+src15) + 16)>>5;
        dst[67] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9+src16) + 16)>>5;
        dst[68] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16) + 16)>>5;
        dst[69] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15) + 16)>>5;
        dst[70] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14) + 16)>>5;
        dst[71] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((ref_src0+ref_src1)*20 - (ref_src0+ref_src2)*6 + (ref_src1+ref_src3)*3 - (ref_src2+ref_src4) + 16)>>5;
        dst[128] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src1+ref_src2)*20 - (ref_src0+ref_src3)*6 + (ref_src0+ref_src4)*3 - (ref_src1+ref_src5) + 16)>>5;
        dst[129] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src2+ref_src3)*20 - (ref_src1+ref_src4)*6 + (ref_src0+ref_src5)*3 - (ref_src0+ref_src6) + 16)>>5;
        dst[130] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src3+ref_src4)*20 - (ref_src2+ref_src5)*6 + (ref_src1+ref_src6)*3 - (ref_src0+ref_src7) + 16)>>5;
        dst[131] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((ref_src4+ref_src5)*20 - (ref_src3+ref_src6)*6 + (ref_src2+ref_src7)*3 - (ref_src1+ref_src8) + 16)>>5;
        dst[132] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src5+ref_src6)*20 - (ref_src4+ref_src7)*6 + (ref_src3+ref_src8)*3 - (ref_src2+ref_src9) + 16)>>5;
        dst[133] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src6+ref_src7)*20 - (ref_src5+ref_src8)*6 + (ref_src4+ref_src9)*3 - (ref_src3+ref_src10) + 16)>>5;
        dst[134] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src7+ref_src8)*20 - (ref_src6+ref_src9)*6 + (ref_src5+ref_src10)*3 - (ref_src4+ref_src11) + 16)>>5;
        dst[135] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((ref_src8+ref_src9)*20 - (ref_src7+ref_src10)*6 + (ref_src6+ref_src11)*3 - (ref_src5+ref_src12) + 16)>>5;
        dst[192] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src9+ref_src10)*20 - (ref_src8+ref_src11)*6 + (ref_src7+ref_src12)*3 - (ref_src6+ref_src13) + 16)>>5;
        dst[193] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src10+ref_src11)*20 - (ref_src9+ref_src12)*6 + (ref_src8+ref_src13)*3 - (ref_src7+ref_src14) + 16)>>5;
        dst[194] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src11+ref_src12)*20 - (ref_src10+ref_src13)*6 + (ref_src9+ref_src14)*3 - (ref_src8+ref_src15) + 16)>>5;
        dst[195] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((ref_src12+ref_src13)*20 - (ref_src11+ref_src14)*6 + (ref_src10+ref_src15)*3 - (ref_src9+ref_src16) + 16)>>5;
        dst[196] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src13+ref_src14)*20 - (ref_src12+ref_src15)*6 + (ref_src11+ref_src16)*3 - (ref_src10+ref_src16) + 16)>>5;
        dst[197] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src14+ref_src15)*20 - (ref_src13+ref_src16)*6 + (ref_src12+ref_src16)*3 - (ref_src11+ref_src15) + 16)>>5;
        dst[198] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((ref_src15+ref_src16)*20 - (ref_src14+ref_src16)*6 + (ref_src13+ref_src15)*3 - (ref_src12+ref_src14) + 16)>>5;
        dst[199] = BYTE_LIMT(ABS(src_temp));
        dst+=dstStride;
        src+=srcStride;
        ref_src+=srcStride;
    }
}

void mp4_qpel16_h_lowpass_16X16(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
    int i;
    int src_temp;
    for(i=0; i<h; i++)
    {
        const int src0= src[0];
        const int src1= src[1];
        const int src2= src[2];
        const int src3= src[3];
        const int src4= src[4];
        const int src5= src[5];
        const int src6= src[6];
        const int src7= src[7];
        const int src8= src[8];
        const int src9= src[9];
        const int src10= src[10];
        const int src11= src[11];
        const int src12= src[12];
        const int src13= src[13];
        const int src14= src[14];
        const int src15= src[15];
        const int src16= src[16];

        src_temp = ((src0+src1)*20 - (src0+src2)*6 + (src1+src3)*3 - (src2+src4) + 16)>>5;
        dst[0] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src1+src2)*20 - (src0+src3)*6 + (src0+src4)*3 - (src1+src5) + 16)>>5;
        dst[1] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src2+src3)*20 - (src1+src4)*6 + (src0+src5)*3 - (src0+src6) + 16)>>5;
        dst[2] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src3+src4)*20 - (src2+src5)*6 + (src1+src6)*3 - (src0+src7) + 16)>>5;
        dst[3] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src4+src5)*20 - (src3+src6)*6 + (src2+src7)*3 - (src1+src8) + 16)>>5;
        dst[4] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src5+src6)*20 - (src4+src7)*6 + (src3+src8)*3 - (src2+src9) + 16)>>5;
        dst[5] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src6+src7)*20 - (src5+src8)*6 + (src4+src9)*3 - (src3+src10) + 16)>>5;
        dst[6] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src7+src8)*20 - (src6+src9)*6 + (src5+src10)*3 - (src4+src11) + 16)>>5;
        dst[7] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src8+src9)*20 - (src7+src10)*6 + (src6+src11)*3 - (src5+src12) + 16)>>5;
        dst[8] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src9+src10)*20 - (src8+src11)*6 + (src7+src12)*3 - (src6+src13) + 16)>>5;
        dst[9] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src10+src11)*20 - (src9+src12)*6 + (src8+src13)*3 - (src7+src14) + 16)>>5;
        dst[10] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src11+src12)*20 - (src10+src13)*6 + (src9+src14)*3 - (src8+src15) + 16)>>5;
        dst[11] = BYTE_LIMT(ABS(src_temp));

        src_temp = ((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9+src16) + 16)>>5;
        dst[12] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16) + 16)>>5;
        dst[13] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15) + 16)>>5;
        dst[14] = BYTE_LIMT(ABS(src_temp));
        src_temp = ((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14) + 16)>>5;
        dst[15] = BYTE_LIMT(ABS(src_temp));

        dst+=dstStride;
        src+=srcStride;
    }
}

void mp4_qpel16_v_lowpass(uint8_t *dst, uint8_t *src, int dstStride, int srcStride)
{
    int i;
    uint8_t *ref_src = src+ 8;
    for(i=0; i<8; i++)
    {
        const int src0= src[0*srcStride];
        const int src1= src[1*srcStride];
        const int src2= src[2*srcStride];
        const int src3= src[3*srcStride];
        const int src4= src[4*srcStride];
        const int src5= src[5*srcStride];
        const int src6= src[6*srcStride];
        const int src7= src[7*srcStride];
        const int src8= src[8*srcStride];
        const int src9= src[9*srcStride];
        const int src10= src[10*srcStride];
        const int src11= src[11*srcStride];
        const int src12= src[12*srcStride];
        const int src13= src[13*srcStride];
        const int src14= src[14*srcStride];
        const int src15= src[15*srcStride];
        const int src16= src[16*srcStride];
        const int ref_src0= ref_src[0*srcStride];
        const int ref_src1= ref_src[1*srcStride];
        const int ref_src2= ref_src[2*srcStride];
        const int ref_src3= ref_src[3*srcStride];
        const int ref_src4= ref_src[4*srcStride];
        const int ref_src5= ref_src[5*srcStride];
        const int ref_src6= ref_src[6*srcStride];
        const int ref_src7= ref_src[7*srcStride];
        const int ref_src8= ref_src[8*srcStride];
        const int ref_src9= ref_src[9*srcStride];
        const int ref_src10= ref_src[10*srcStride];
        const int ref_src11= ref_src[11*srcStride];
        const int ref_src12= ref_src[12*srcStride];
        const int ref_src13= ref_src[13*srcStride];
        const int ref_src14= ref_src[14*srcStride];
        const int ref_src15= ref_src[15*srcStride];
        const int ref_src16= ref_src[16*srcStride];
        dst[0*dstStride] = BYTE_LIMT(ABS(((src0 +src1 )*20 - (src0 +src2 )*6 + (src1 +src3 )*3 - (src2 +src4 ) + 16)>>5));
        dst[1*dstStride] = BYTE_LIMT(ABS(((src1 +src2 )*20 - (src0 +src3 )*6 + (src0 +src4 )*3 - (src1 +src5 ) + 16)>>5));
        dst[2*dstStride] = BYTE_LIMT(ABS(((src2 +src3 )*20 - (src1 +src4 )*6 + (src0 +src5 )*3 - (src0 +src6 ) + 16)>>5));
        dst[3*dstStride] = BYTE_LIMT(ABS(((src3 +src4 )*20 - (src2 +src5 )*6 + (src1 +src6 )*3 - (src0 +src7 ) + 16)>>5));
        dst[4*dstStride] = BYTE_LIMT(ABS(((src4 +src5 )*20 - (src3 +src6 )*6 + (src2 +src7 )*3 - (src1 +src8 ) + 16)>>5));
        dst[5*dstStride] = BYTE_LIMT(ABS(((src5 +src6 )*20 - (src4 +src7 )*6 + (src3 +src8 )*3 - (src2 +src9 ) + 16)>>5));
        dst[6*dstStride] = BYTE_LIMT(ABS(((src6 +src7 )*20 - (src5 +src8 )*6 + (src4 +src9 )*3 - (src3 +src10) + 16)>>5));
        dst[7*dstStride] = BYTE_LIMT(ABS(((src7 +src8 )*20 - (src6 +src9 )*6 + (src5 +src10)*3 - (src4 +src11) + 16)>>5));

        dst[128 + 0*dstStride] = BYTE_LIMT(ABS(((src8 +src9 )*20 - (src7 +src10)*6 + (src6 +src11)*3 - (src5 +src12) + 16)>>5));
        dst[128 + 1*dstStride] = BYTE_LIMT(ABS(((src9 +src10)*20 - (src8 +src11)*6 + (src7 +src12)*3 - (src6 +src13) + 16)>>5));
        dst[128 + 2*dstStride] = BYTE_LIMT(ABS(((src10+src11)*20 - (src9 +src12)*6 + (src8 +src13)*3 - (src7 +src14) + 16)>>5));
        dst[128 + 3*dstStride] = BYTE_LIMT(ABS(((src11+src12)*20 - (src10+src13)*6 + (src9 +src14)*3 - (src8 +src15) + 16)>>5));
        dst[128 + 4*dstStride] = BYTE_LIMT(ABS(((src12+src13)*20 - (src11+src14)*6 + (src10+src15)*3 - (src9 +src16) + 16)>>5));
        dst[128 + 5*dstStride] = BYTE_LIMT(ABS(((src13+src14)*20 - (src12+src15)*6 + (src11+src16)*3 - (src10+src16) + 16)>>5));
        dst[128 + 6*dstStride] = BYTE_LIMT(ABS(((src14+src15)*20 - (src13+src16)*6 + (src12+src16)*3 - (src11+src15) + 16)>>5));
        dst[128 + 7*dstStride] = BYTE_LIMT(ABS(((src15+src16)*20 - (src14+src16)*6 + (src13+src15)*3 - (src12+src14) + 16)>>5));

        dst[64 + 0*dstStride]  = BYTE_LIMT(ABS(((ref_src0 +ref_src1 )*20 - (ref_src0 +ref_src2 )*6 + (ref_src1 +ref_src3 )*3 - (ref_src2 +ref_src4 ) + 16)>>5));
        dst[64 + 1*dstStride]  = BYTE_LIMT(ABS(((ref_src1 +ref_src2 )*20 - (ref_src0 +ref_src3 )*6 + (ref_src0 +ref_src4 )*3 - (ref_src1 +ref_src5 ) + 16)>>5));
        dst[64 + 2*dstStride]  = BYTE_LIMT(ABS(((ref_src2 +ref_src3 )*20 - (ref_src1 +ref_src4 )*6 + (ref_src0 +ref_src5 )*3 - (ref_src0 +ref_src6 ) + 16)>>5));
        dst[64 + 3*dstStride]  = BYTE_LIMT(ABS(((ref_src3 +ref_src4 )*20 - (ref_src2 +ref_src5 )*6 + (ref_src1 +ref_src6 )*3 - (ref_src0 +ref_src7 ) + 16)>>5));
        dst[64 + 4*dstStride]  = BYTE_LIMT(ABS(((ref_src4 +ref_src5 )*20 - (ref_src3 +ref_src6 )*6 + (ref_src2 +ref_src7 )*3 - (ref_src1 +ref_src8 ) + 16)>>5));
        dst[64 + 5*dstStride]  = BYTE_LIMT(ABS(((ref_src5 +ref_src6 )*20 - (ref_src4 +ref_src7 )*6 + (ref_src3 +ref_src8 )*3 - (ref_src2 +ref_src9 ) + 16)>>5));
        dst[64 + 6*dstStride]  = BYTE_LIMT(ABS(((ref_src6 +ref_src7 )*20 - (ref_src5 +ref_src8 )*6 + (ref_src4 +ref_src9 )*3 - (ref_src3 +ref_src10) + 16)>>5));
        dst[64 + 7*dstStride]  = BYTE_LIMT(ABS(((ref_src7 +ref_src8 )*20 - (ref_src6 +ref_src9 )*6 + (ref_src5 +ref_src10)*3 - (ref_src4 +ref_src11) + 16)>>5));

        dst[192 + 0*dstStride] = BYTE_LIMT(ABS(((ref_src8  +ref_src9    )*20 - (ref_src7  +ref_src10 )*6 + (ref_src6  +ref_src11)*3 - (ref_src5  +ref_src12) + 16)>>5));
        dst[192 + 1*dstStride] = BYTE_LIMT(ABS(((ref_src9  +ref_src10 )*20 - (ref_src8  +ref_src11 )*6 + (ref_src7  +ref_src12)*3 - (ref_src6  +ref_src13) + 16)>>5));
        dst[192 + 2*dstStride] = BYTE_LIMT(ABS(((ref_src10 +ref_src11 )*20 - (ref_src9  +ref_src12 )*6 + (ref_src8  +ref_src13)*3 - (ref_src7  +ref_src14) + 16)>>5));
        dst[192 + 3*dstStride] = BYTE_LIMT(ABS(((ref_src11 +ref_src12 )*20 - (ref_src10 +ref_src13 )*6 + (ref_src9  +ref_src14)*3 - (ref_src8  +ref_src15) + 16)>>5));
        dst[192 + 4*dstStride] = BYTE_LIMT(ABS(((ref_src12 +ref_src13 )*20 - (ref_src11 +ref_src14 )*6 + (ref_src10 +ref_src15)*3 - (ref_src9  +ref_src16) + 16)>>5));
        dst[192 + 5*dstStride] = BYTE_LIMT(ABS(((ref_src13 +ref_src14 )*20 - (ref_src12 +ref_src15 )*6 + (ref_src11 +ref_src16)*3 - (ref_src10 +ref_src16) + 16)>>5));
        dst[192 + 6*dstStride] = BYTE_LIMT(ABS(((ref_src14 +ref_src15 )*20 - (ref_src13 +ref_src16 )*6 + (ref_src12 +ref_src16)*3 - (ref_src11 +ref_src15) + 16)>>5));
        dst[192 + 7*dstStride] = BYTE_LIMT(ABS(((ref_src15 +ref_src16 )*20 - (ref_src14 +ref_src16 )*6 + (ref_src13 +ref_src15)*3 - (ref_src12 +ref_src14) + 16)>>5));

        dst++;
        src++;
        ref_src++;
    }
}

static void copy_block9(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
    int i;
    for(i=0; i<h; i++)
    {
        AV_WN32(dst   , AV_RN32(src   ));
        AV_WN32(dst+4 , AV_RN32(src+4 ));
        dst[8]= src[8];
        dst+=dstStride;
        src+=srcStride;
    }
}

static void copy_block17(uint8_t *dst, uint8_t *src, int dstStride, int srcStride, int h)
{
    int i;
    for(i=0; i<h; i++)
    {
        AV_WN32(dst   , AV_RN32(src   ));
        AV_WN32(dst+4 , AV_RN32(src+4 ));
        AV_WN32(dst+8 , AV_RN32(src+8 ));
        AV_WN32(dst+12, AV_RN32(src+12));
        dst[16]= src[16];
        dst+=dstStride;
        src+=srcStride;
    }
}

void mp4_qpel16_mc10_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[256];
    mp4_qpel16_h_lowpass(half, src, 8, stride, 8);
    put_mp4_pixels16_l2_8(dst, src, half, stride);
}

void mp4_qpel16_mc20_c(uint8_t *dst, uint8_t *src, int stride)
{
    mp4_qpel16_h_lowpass(dst, src, 8, stride, 8);
}

void mp4_qpel16_mc30_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[256];
    mp4_qpel16_h_lowpass(half, src, 8, stride, 8);
    put_mp4_pixels16_l2_8(dst, src+1, half, stride);
}

void mp4_qpel16_mc01_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    uint8_t half[256];
    //copy_block17(full, src, 24, stride, 17);
    mp4_qpel16_v_lowpass(half, src, 8, stride);
    put_mp4_pixels16_l2_8(dst, src, half, stride);
}

void mp4_qpel16_mc02_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    //copy_block17(full, src, 24, stride, 17);
    mp4_qpel16_v_lowpass(dst, src, 8, stride);
}

void mp4_qpel16_mc03_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    uint8_t half[256];
    //copy_block17(full, src, 24, stride, 17);
    mp4_qpel16_v_lowpass(half, src, 8, stride);
    put_mp4_pixels16_l2_8(dst, src+stride, half, stride);

}

void mp4_qpel16_mc11_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    uint8_t halfH[272];
    uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(temp_halfHV, src, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);
    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src, halfH, stride, 17);
    mp4_qpel16_v_lowpass(halfHV, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH, halfHV, 16);
}

void mp4_qpel16_mc31_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    uint8_t halfH[272];
    uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(halfH, halfH, src+1, stride);
    //mp4_qpel16_v_lowpass(temp_halfHV, halfH, 8, stride);
    //mp4_chang_array8to16(halfHV,temp_halfHV);
    //put_mp4_pixels16_l2_8(dst, halfH, halfHV, 16);

    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    // put_mp4_pixels16_l2_8(temp_halfHV, src+1, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);

    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src+1, halfH, stride, 17);
    mp4_qpel16_v_lowpass(halfHV, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH, halfHV, 16);

}

void mp4_qpel16_mc13_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    //uint8_t halfH[272];
    //uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(halfH, halfH, src, stride);
    //mp4_qpel16_v_lowpass(halfHV, halfH, 8, stride);
    //put_mp4_pixels16_l2_8(dst, halfH+16, halfHV, stride);

    //uint8_t full[24*17];
    uint8_t halfH[272];
    uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(temp_halfHV, src, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);

    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src, halfH, stride, 17);
    mp4_qpel16_v_lowpass(halfHV, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH+16, halfHV, 16);

}

void mp4_qpel16_mc33_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    //uint8_t halfH[272];
    //uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, full, 16, 24, 17);
    //put_mp4_pixels16_l2_8(halfH, halfH, full+1, 16, 16, 24, 17);
    //mp4_qpel16_v_lowpass(halfHV, halfH, 16, 16);
    //put_mp4_pixels16_l2_8(dst, halfH+16, halfHV, stride, 16, 16, 16);

    uint8_t halfH[272];
    uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(temp_halfHV, src+1, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);
    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src+1, halfH, stride, 17);
    mp4_qpel16_v_lowpass(halfHV, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH+16, halfHV, 16);


}
void mp4_qpel16_mc21_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[272];
    //uint8_t halfHV[256];
    //mp4_qpel16_h_lowpass(halfH, src, 16, stride, 17);
    //mp4_qpel16_v_lowpass(halfHV, halfH, 16, 16);
    //put_mp4_pixels16_l2_8(dst, halfH, halfHV, stride, 16, 16, 16);

    uint8_t halfH[272];
    uint8_t halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(temp_halfHV, src, 8, stride, 8);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);
    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    //put_mp4_pixels_16X16(halfH, src+1, halfH, stride, 17);
    mp4_qpel16_v_lowpass(halfHV, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH, halfHV, 16);

}
void mp4_qpel16_mc23_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[272];
    //uint8_t halfHV[256];
    //mp4_qpel16_h_lowpass(halfH, src, 16, stride, 17);
    //mp4_qpel16_v_lowpass(halfHV, halfH, 16, 16);
    //put_mp4_pixels16_l2_8(dst, halfH+16, halfHV, stride, 16, 16, 16);

    uint8_t halfH[272];
    uint8_t half_temp[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(half_trmp, src, 8, stride, 8);
    //mp4_chang_array8to16(halfH,half_trmp,16);
    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    //put_mp4_pixels_16X16(halfH, src+1, halfH, stride, 17);
    mp4_qpel16_v_lowpass(half_temp, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    put_mp4_pixels16_l2_8(dst, halfH+16, half_temp, 16);

}
void mp4_qpel16_mc12_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    //uint8_t halfH[272];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, full, 16, 24, 17);
    //put_mp4_pixels16_l2_8(halfH, halfH, full, 16, 16, 24, 17);
    //mp4_qpel16_v_lowpass(dst, halfH, stride, 16);


    uint8_t halfH[272];
    //uint8_t temp_halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(temp_halfHV, src, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);
    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src, halfH, stride, 17);
    mp4_qpel16_v_lowpass(dst, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    //put_mp4_pixels16_l2_8(dst, halfH, temp_halfHV, 16);



}
void mp4_qpel16_mc32_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[24*17];
    //uint8_t halfH[272];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, full, 16, 24, 17);
    //put_mp4_pixels16_l2_8(halfH, halfH, full+1, 16, 16, 24, 17);
    //mp4_qpel16_v_lowpass(dst, halfH, stride, 16);

    uint8_t halfH[272];
    //uint8_t temp_halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(halfH, src, 8, stride, 8);

    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    put_mp4_pixels_16X16(halfH, src+1, halfH, stride, 17);
    //put_mp4_pixels16_l2_8(temp_halfHV, src+1, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);
    mp4_qpel16_v_lowpass(dst, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    //put_mp4_pixels16_l2_8(dst, halfH, temp_halfHV, 16);
}
void mp4_qpel16_mc22_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[272];
    //mp4_qpel16_h_lowpass(halfH, src, 16, stride, 17);
    //mp4_qpel16_v_lowpass(dst, halfH, stride, 16);

    uint8_t halfH[272];
    //uint8_t temp_halfHV[256];
    //copy_block17(full, src, 24, stride, 17);
    //mp4_qpel16_h_lowpass(temp_halfHV, src, 8, stride, 8);
    //put_mp4_pixels16_l2_8(temp_halfHV, src, halfH, stride);
    //mp4_chang_array8to16(halfH,temp_halfHV,16);

    mp4_qpel16_h_lowpass_16X16(halfH, src, 16, stride, 17);
    mp4_qpel16_v_lowpass(dst, halfH, 8, 16);
    //mp4_chang_array8to16(halfHV,temp_halfHV, 16);
    //put_mp4_pixels16_l2_8(dst, halfH, temp_halfHV, 16);
}


void mp4_qpel8_mc10_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[64];
    mp4_qpel8_h_lowpass(half, src, 8, stride, 8);
    put_mp4_pixels8_l2_8(dst, src, half, 8, stride, 8, 8);
}

void mp4_qpel8_mc20_c(uint8_t *dst, uint8_t *src, int stride)
{
    mp4_qpel8_h_lowpass(dst, src, 8, stride, 8);
}

void mp4_qpel8_mc30_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[64];
    mp4_qpel8_h_lowpass(half, src, 8, stride, 8);
    put_mp4_pixels8_l2_8(dst, src+1, half, 8, stride, 8, 8);
}

void mp4_qpel8_mc01_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t half[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_v_lowpass(half, src, 8, stride);
    put_mp4_pixels8_l2_8(dst, src, half, 8, stride, 8, 8);
}

void mp4_qpel8_mc02_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_v_lowpass(dst, src, 8, stride);
}

void mp4_qpel8_mc03_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t half[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_v_lowpass(half, src, 8, stride);
    put_mp4_pixels8_l2_8(dst, src+stride, half, 8, stride, 8, 8);
}
void mp4_qpel8_mc11_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, src, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_mc31_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, src+1, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_mc13_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, src, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_mc33_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full+1, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, src+1, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8, 8);
}
void mp4_qpel8_mc21_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}
void mp4_qpel8_mc23_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8 ,8);
}

void mp4_qpel8_mc12_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(dst, halfH, stride, 8);

    uint8_t halfH[72];
    //uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, halfH, src, 8, 8, stride, 9);
    mp4_qpel8_v_lowpass(dst, halfH, 8, 8);
}

void mp4_qpel8_mc32_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_pixels8_l2_8(halfH, halfH, src+1, 8, 8, stride, 9);
    mp4_qpel8_v_lowpass(dst, halfH, 8, 8);
}
void mp4_qpel8_mc22_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t halfH[72];
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(dst, halfH, 8, 8);
}

void mp4_qpel8_add_mc10_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[64];
    mp4_qpel8_h_lowpass(half, src, 8, stride, 8);
    put_mp4_add_pixels8_l2_8(dst, src, half, 8, stride, 8, 8);
}

void mp4_qpel8_add_mc20_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8 halfHV[64];
    mp4_qpel8_h_lowpass(halfHV, src, 8, stride, 8);
    put_mp4_add_pixels8_l2_8(dst, halfHV, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc30_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t half[64];
    mp4_qpel8_h_lowpass(half, src, 8, stride, 8);
    put_mp4_add_pixels8_l2_8(dst, src+1, half, 8, stride, 8, 8);
}

void mp4_qpel8_add_mc01_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t half[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_v_lowpass(half, src, 8, stride);
    put_mp4_add_pixels8_l2_8(dst, src, half, 8, stride, 8, 8);
}

void mp4_qpel8_add_mc02_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //copy_block9(full, src, 16, stride, 9);
    uint8 halfHV[64];
    mp4_qpel8_v_lowpass(halfHV, src, 8, stride);
    put_mp4_add_pixels8_l2_8(dst, halfHV, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc03_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t half[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_v_lowpass(half, src, 8, stride);
    put_mp4_add_pixels8_l2_8(dst, src+stride, half, 8, stride, 8, 8);
}
void mp4_qpel8_add_mc11_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, src, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc31_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, src+1, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc13_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, src, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc33_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full+1, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, src+1, halfH, 8, stride, 8, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8, 8);
}
void mp4_qpel8_add_mc21_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH, halfHV, 8, 8, 8, 8);
}
void mp4_qpel8_add_mc23_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t halfH[72];
    //uint8_t halfHV[64];
    //mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    //mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    //put_mp4_pixels8_l2_8(dst, halfH+8, halfHV, stride, 8, 8, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfH+8, halfHV, 8, 8, 8 ,8);
}

void mp4_qpel8_add_mc12_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    //uint8_t halfH[72];
    //copy_block9(full, src, 16, stride, 9);
    //mp4_qpel8_h_lowpass(halfH, full, 8, 16, 9);
    //put_mp4_pixels8_l2_8(halfH, halfH, full, 8, 8, 16, 9);
    //mp4_qpel8_v_lowpass(dst, halfH, stride, 8);

    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, halfH, src, 8, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfHV, halfHV, 8, 8, 8, 8);
}

void mp4_qpel8_add_mc32_c(uint8_t *dst, uint8_t *src, int stride)
{
    //uint8_t full[16*9];
    uint8_t halfH[72];
    uint8_t halfHV[64];
    //copy_block9(full, src, 16, stride, 9);
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    put_mp4_add_pixels8_l2_8(halfH, halfH, src+1, 8, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfHV, halfHV, 8, 8, 8, 8);
}
void mp4_qpel8_add_mc22_c(uint8_t *dst, uint8_t *src, int stride)
{
    uint8_t halfH[72];
    uint8_t halfHV[64];
    mp4_qpel8_h_lowpass(halfH, src, 8, stride, 9);
    mp4_qpel8_v_lowpass(halfHV, halfH, 8, 8);
    put_mp4_add_pixels8_l2_8(dst, halfHV, halfHV, 8, 8, 8, 8);
}
#endif
#endif
#endif// MEDIA_VOCVID_SUPPORT
