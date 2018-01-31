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


typedef int32_t sbc_fixed_t;

#define fabs(x) ((x) < 0 ? -(x) : (x))
/* C does not provide an explicit arithmetic shift right but this will
   always be correct and every compiler *should* generate optimal code */
#define ASR(val, bits) ((-2 >> 1 == -1) ? \
         ((int32_t)(val)) >> (bits) : ((int32_t) (val)) / (1 << (bits)))

#define SCALE_PROTO4_TBL    15
#define SCALE_ANA4_TBL      17
#define SCALE_PROTO8_TBL    16
#define SCALE_ANA8_TBL      17
#define SCALE_SPROTO4_TBL   12
#define SCALE_SPROTO8_TBL   14
#define SCALE_NPROTO4_TBL   11
#define SCALE_NPROTO8_TBL   11
#define SCALE4_STAGE1_BITS  15
#define SCALE4_STAGE2_BITS  16
#define SCALE4_STAGED1_BITS 15
#define SCALE4_STAGED2_BITS 16
#define SCALE8_STAGE1_BITS  15
#define SCALE8_STAGE2_BITS  15
#define SCALE8_STAGED1_BITS 15
#define SCALE8_STAGED2_BITS 16


#ifdef _VC_DEBUG_
typedef short int16_t;
#endif
#define SCALE4_STAGE1(src)  ASR(src, SCALE4_STAGE1_BITS)
#define SCALE4_STAGE2(src)  ASR(src, SCALE4_STAGE2_BITS)
#define SCALE4_STAGED1(src) ASR(src, SCALE4_STAGED1_BITS)
#define SCALE4_STAGED2(src) ASR(src, SCALE4_STAGED2_BITS)
#define SCALE8_STAGE1(src)  ASR(src, SCALE8_STAGE1_BITS)
#define SCALE8_STAGE2(src)  ASR(src, SCALE8_STAGE2_BITS)
#define SCALE8_STAGED1(src) ASR(src, SCALE8_STAGED1_BITS)
#define SCALE8_STAGED2(src) ASR(src, SCALE8_STAGED2_BITS)

#define SBC_FIXED_0(val) { val = 0; }
#define MUL(a, b)        ((a) * (b))
#ifdef __arm__
#define MULA(a, b, res) ({              \
        int tmp = res;          \
        __asm__(                \
            "mla %0, %2, %3, %0"        \
            : "=&r" (tmp)           \
            : "0" (tmp), "r" (a), "r" (b)); \
        tmp; })
#else
#define MULA(a, b, res)  ((a) * (b) + (res))
#endif
