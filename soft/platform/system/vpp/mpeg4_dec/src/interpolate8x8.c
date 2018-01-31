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





#include "portab.h"
#include "mpeg4_global.h"
#include "interpolate8x8.h"

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

