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



#ifndef __VPP_EFR_BASIC_MACRO_H__
#define __VPP_EFR_BASIC_MACRO_H__

# define       MAD_F_SCALEBITS      30

#define ADD(var1,var2)                           (var1 + var2)
#define ADD_SAT16(var1, var2)                             \
    (((INT32)(var1)+(INT32)(var2))> MAX_16) ? MAX_16 :  \
    (((INT32)(var1)+(INT32)(var2))< MIN_16) ? MIN_16 :  \
      (INT16)((INT32)(var1)+(INT32)(var2))

#define SUB(var1, var2)                         (var1 - var2)

#define SHL(var1, var2)                         (var1 << var2)
#define SHL_SAT16(var1,var2)                                \
    (((INT32)(var1)<<(INT32)(var2)) > MAX_16) ? MAX_16 :  \
    (((INT32)(var1)<<(INT32)(var2)) < MIN_16) ? MIN_16 :  \
      (INT16)((INT32)(var1)<<(INT32)(var2))


#define ABS_S(var1)                          ((var1 == MIN_16)?MAX_16:((var1 > 0)? var1: -var1))
#define L_ABS(var1)                          ((var1 == MIN_32)?MAX_32:((var1 > 0)? var1: -var1))

#define NEGATE(var1)                         ((var1 == MIN_16)?MAX_16:(-var1))
#define L_NEGATE(var1)                       ((var1 == MIN_32)?MAX_32:(-var1))

#define EXTRACT_H(var1)                      ((INT16)(var1 >> 16))
#define EXTRACT_L(var1)                      ((INT16)(var1))

#define L_ADD(var1,var2)                                              \
     ((!((var1 ^ var2) & MIN_32))&&(((var1+var2) ^ var1) & MIN_32))?  \
     ((var1 < 0) ? MIN_32 : MAX_32):                                  \
     (var1+var2)

#define L_SUB(var1, var2)\
        (((var1 ^ var2) & MIN_32)&&(((var1 - var2)^var1)& MIN_32))?\
     ((var1 < 0) ? MIN_32 : MAX_32):\
     (var1 - var2)



#define L_DEPOSIT_H(var1)                    ((INT32)(var1 << 16))
#define L_DEPOSIT_L(var1)                    ((INT32)var1)


#define ROUND(var1)                          EXTRACT_H(L_ADD(var1,(INT32) 0x00008000L))                        \

#define SHR_D(var1, var2)                    (var1 >> var2)
#define SHR_V(var1, var2)                     \
       ((var2 < 0) ? SHL_SAT16(var1, -var2):(var1 >> var2))

#define MULT(var1, var2)                     (INT16)(((INT32) var1 *(INT32) var2)>> 15)

#define L_MULT(var1, var2)\
   (((var1==((INT16)0x8000))&&(var2==(INT16)0x8000)) ? MAX_32 :\
    ((INT32)((INT32)var1*(INT32)var2)<<1))

/*#define MULT_R(var1, var2)\
(INT16)((((INT32) var1 *(INT32) var2) + (INT32) 0x00004000L) >> 15)*/



#define L_SHL(var1, var2)                             (var1 << var2)
#define L_SHL_SAT(var1, var2)\
    (((var1 <0)&& ((var1 & (MIN_32 >>var2))!= (MIN_32 >>var2)))?MIN_32:\
    (((var1 >0)&& ((var1 | (MAX_32 >>var2))!= (MAX_32 >>var2)))? MAX_32:(var1 << var2)))

#define L_SHR_D(var1, var2)                   (var1 >> var2)
#define L_SHR_V(var1, var2)\
    ((var2 < 0)? L_SHL_SAT(var1, (ABS_S(var2))): (var1 >> var2))


#ifdef TARGET_SIMU


#   define VPP_MLX(hi, lo, x, y)  \
    {\
    register int __x = (int)(x); \
    register int __y = (int)(y); \
    asm ("imull %3"  \
     : "=a" (lo), "=d" (hi)  \
     : "%a" (__x), "rm" (__y)  \
     : "cc");\
    }

#   define VPP_MLA(hi, lo, x, y)  \
    ({ INT32 __hi;  \
       UINT32 __lo;  \
       VPP_MLX16(__hi, __lo, (x), (y));  \
       asm ("addl %2,%0\n\t"  \
        "adcl %3,%1"  \
        : "=rm" (lo), "=rm" (hi)  \
        : "r" (__lo), "r" (__hi), "0" (lo), "1" (hi)  \
        : "cc");  \
    })

#   define VPP_MLA16(hi, lo, x, y)        VPP_MLA(hi, lo, x, y)
#   define VPP_MLX16(hi, lo, x, y)        VPP_MLX(hi, lo, x, y)


#   define VPP_BIMLA(hi, lo, x, y)  \
    ({ \
        VPP_MLA(hi, lo, ((x << 16) >> 16), ((y << 16) >> 16));  \
        VPP_MLA(hi, lo, ((x      ) >> 16), ((y      ) >> 16));  \
    })


#   define VPP_SCALE64_TO_16(hi, lo)  (INT32) (lo << 1)

#   define VPP_MULT_R(hi, lo, x, y)\
     ({ lo = 0x00004000L;  \
        VPP_MLA16(hi,lo,x, y);\
    })


#else

#   define VPP_MLX(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
     : "=l" (lo), "=h" (hi)  \
     : "%r" (x), "r" (y))

#   define VPP_MLX16(hi, lo, x, y)  \
    ({ \
    register int __x = (int)x; \
    register int __y = (int)y; \
    asm ("mult	%2,%3"  \
     : "=l" (lo), "=h" (hi)  \
     : "%r" (__x), "r" (__y));\
})

#   define VPP_MLA(hi, lo, x, y)  \
    asm ("madd	%2,%3"  \
     : "+l" (lo), "+h" (hi)  \
     : "%r" (x), "r" (y))

#   define VPP_MLA16(hi, lo, x, y)  \
    ({ \
    register int __x = (int)x; \
    register int __y = (int)y; \
    asm ("madd	%2,%3"  \
     : "+l" (lo), "+h" (hi)  \
     : "%r" (__x), "r" (__y)); \
    })

#   define VPP_BIMLA(hi, lo, x, y)  \
    ({ \
        VPP_MLA(hi, lo, ((x << 16) >> 16), ((y << 16) >> 16));  \
        VPP_MLA(hi, lo, ((x      ) >> 16), ((y      ) >> 16));  \
    })


#   define VPP_SCALE64_TO_16(hi, lo)  (INT32)(lo << 1)

#   define VPP_MULT_R(hi, lo, x, y)\
     ({ lo = 0x00004000L;  \
        VPP_MLA16(hi,lo,x, y);\
    })



#endif




#endif




























