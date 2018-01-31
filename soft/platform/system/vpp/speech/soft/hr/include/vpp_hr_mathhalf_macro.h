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




#ifndef VPP_HR_MATHHALF_MACRO_H_INCLUDED
#define VPP_HR_MATHHALF_MACRO_H_INCLUDED

#define TARGET_SIMU

//static INT16 saturate(INT32 L_var1)
#define SATURATE(L_var1)         \
    (L_var1> SW_MAX) ? SW_MAX :  \
    (L_var1< SW_MIN) ? SW_MIN :  \
    (INT16)(L_var1)


#define L_SATURATE(L_var1)       \
    ((L_var1)> LW_MAX) ? LW_MAX :  \
    ((INT32)(L_var1)< LW_MIN) ? LW_MIN :  \
     (INT32)(L_var1)



//INT16 abs_s(INT16 var1)
#define  ABS_S(var1)  ((var1 == SW_MIN)?SW_MAX:((var1 > 0)? var1: -var1))


//INT16 add(INT16 var1, INT16 var2)
#define ADD(var1,var2)   (var1 + var2)

/*#define ADD_SAT16(var1, var2)                         \
    (((INT32)(var1)+(INT32)(var2))> SW_MAX) ? SW_MAX :  \
    (((INT32)(var1)+(INT32)(var2))< SW_MIN) ? SW_MIN :  \
      (INT16)((INT32)(var1)+(INT32)(var2))*/


//INT16 divide_s(INT16 var1, INT16 var2)
#define DIVIDE_SAT16(var1, var2)   \
   (var1 < 0 || var2 < 0 || var1 > var2)?0:\
   (var1 == var2)?0x7fff:\
   (((0x00008000L * (INT32) var1) / (INT32) var2)> SW_MAX) ? SW_MAX :  \
   (((0x00008000L * (INT32) var1) / (INT32) var2)< SW_MIN) ? SW_MIN :  \
    ((0x00008000L * (INT32) var1) / (INT32) var2)


//INT16 extract_h(INT32 L_var1)
#define EXTRACT_H(L_var1)   ((INT16) (0x0000ffffL & (L_var1 >> 16)))

//INT16 extract_l(INT32 L_var1)
#define EXTRACT_L(L_var1)   ((INT16) (0x0000ffffL & L_var1))

//INT32 L_abs(INT32 L_var1)
#define L_ABS(L_var1)  ((L_var1 == LW_MIN)?LW_MAX:((L_var1 > 0)? L_var1: -L_var1))


//INT32 L_add(INT32 L_var1, INT32 L_var2)
#define L_ADD(L_var1,L_var2)                                              \
     ((!((L_var1 ^ L_var2) & LW_MIN))&&(((L_var1+L_var2) ^ L_var1) & LW_MIN))?  \
     ((L_var1 < 0) ? LW_MIN : LW_MAX):   \
     (L_var1+L_var2)



#define L_MAC( L_var3,  var1,  var2)   (L_ADD(L_var3, (L_MULT(var1, var2))))

#define L_MSU( L_var3,  var1,  var2)   (L_SUB(L_var3, (L_MULT(var1, var2))))

//INT32 L_deposit_h(INT16 var1)
#define L_DEPOSIT_H(var1)   ((INT32) (var1 << 16))

//INT32 L_deposit_l(INT16 var1)
#define L_DEPOSIT_L(var1)   ((INT32)(var1))

//INT32 L_mult(INT16 var1, INT16 var2)
#define L_MULT(var1, var2)   \
   ((var1 == SW_MIN && var2 == SW_MIN))? LW_MAX : ((INT32) ((INT32) var1 *var2)<<1)


//INT32 L_negate(INT32 L_var1)
#define L_NEGATE(L_var1)   ((L_var1 == LW_MIN)? LW_MAX : (-L_var1))





//INT32 L_sub(INT32 L_var1, INT32 L_var2)
#define L_SUB(var1, var2)\
     (((var1 ^ var2) & LW_MIN)&&(((var1 - var2)^var1)& LW_MIN))?\
     ((var1 < 0) ? LW_MIN : LW_MAX):\
     (var1 - var2)


//INT16 sub(INT16 var1, INT16 var2)
//#define SUB_s(var1 , var2)   ( SATURATE((INT32) var1 - var2) )
#define SUB(var1,var2)       (var1 - var2)




//INT16 negate(INT32 L_var1)
#define NEGATE(var1)   ((var1 == SW_MIN)? SW_MAX : (-var1))



//INT16 shl(INT16 var1, INT16 var2)
#define SHL(var1, var2)                         (var1 << var2)

#define SHL_g(var1, var2)  \
    (((INT32) var1 *(1 << var2))!= (INT16)((INT32) var1 *(1 << var2)))?( (var1 > 0)?SW_MAX:SW_MIN ): \
    (INT16)((INT32) var1 *(1 << var2))


//INT32 L_shl(INT32 L_var1, INT16 var2)
#define L_SHL_SAT(var1, var2)\
    (((var1 <0)&& ((var1 & (LW_MIN >>var2))!= (LW_MIN >>var2)))?LW_MIN:\
    (((var1 >0)&& ((var1 | (LW_MAX >>var2))!= (LW_MAX >>var2)))? LW_MAX:(var1 << var2)))




//INT32 L_shr(INT32 L_var1, INT16 var2)
/*#define L_SHR( L_var1,  var2)   \
   (var2 == 0 || L_var1 == 0)? L_var1 : \
   (   (var2 < 0)? (  (var2 <= -31)? ( (L_var1 > 0)? LW_MAX: LW_MIN ): L_shl(L_var1, -var2)  ) :    \
   (var2 >= 31)? ( (L_var1 > 0)?0:0xffffffffL ) : \
   ( (L_var1 < 0)? (((~0) << (32 - var2))|(L_var1 >>= var2)): ((L_var1 < 0)? (0 << (32 - var2))|(L_var1 >>= var2)) )  )*/

#define SHR(var1, var2)     (var1 >> var2)

#define L_SHR_V(var1, var2)\
    ((var2 < 0)? L_SHL_SAT(var1, (ABS_S(var2))): (var1 >> var2))

#define SHR_V(var1, var2)     (var2<0?SHR(var1, -var2):SHL(var1, var2))




//INT16 shift_r(INT16 var1, INT16 var2)
/*#define EESHIFT_R( var1,  var2)   \
    (var2 >= 0)? shl(var1, var2) : (  (var2 < -15)? 0 : ADD((SHR(var1, -var2)), (SHR(var1, (-var2 - 1)) & 0x1))  )*/
#define SHIFT_R( var1,  var2)   ( ADD((SHR(var1, -var2)), (SHR(var1, (-var2 - 1)) & 0x1)) )





//INT16 mult(INT16 var1, INT16 var2)
#define MULT(var1, var2)       ((INT16) EXTRACT_H( ((INT32) L_MULT(var1, var2))))


//INT16 roundo(INT32 L_var1)
#define ROUNDO( L_var1)    ((INT16)(EXTRACT_H( (INT32) (L_ADD(L_var1, 0x00008000L)) )))//

//((INT32) EXTRACT_H((INT32) L_ADD(L_var1,0x00008000L)))//



#define MAC_R(L_var3, var1, var2)      (ROUNDO(L_MAC(L_var3, var1, var2)))

#define MSU_R(L_var3, var1, var2)      (ROUNDO(L_MSU(L_var3, var1, var2)))


#define MULT_R(var1, var2)    (ROUNDO(L_MULT(var1, var2)))






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

# define L_MLA_CARRY_SAT(hi, lo, carry)\
    ((hi ==  0) && ((lo & 0xc0000000) ==  0)) ? ((lo << 1) + (carry)) : \
    ((hi == -1) && ((lo | 0x3fffffff) == -1)) ? ((lo << 1) + (carry)) : \
     (hi >=  0)                             ? LW_MAX : \
                                              LW_MIN


#   define L_MLA_SAT_CARRY(lo, carry)\
    ( (   lo>=LW_MAX)        && (((INT32)(lo<<1))>      0))   ? LW_MAX:\
    ( ( ((lo<<1)&0x80000000) && (((INT32)(lo   ))< LW_MIN))   ? LW_MIN:\
      ( ((INT32)(lo))< 0  )?(lo << 1) + (carry): L_SATURATE( (UINT32)(lo<<1) + (carry) ) )


/*#   define L_MLA_SAT(hi, lo)\
    ( (   lo>=LW_MAX)        && (((INT32)(lo<<1))>      0))   ? LW_MAX:\
    ( ( ((lo<<1)&0x80000000) && (((INT32)(lo   ))< LW_MIN))   ? LW_MIN:\
      ( ((INT32)(lo))< 0  )?(lo << 1): L_SATURATE( (UINT32)(lo<<1) ) )*/

/*if ( (s_lo>=LW_MAX) && ( ((INT32)(s_lo<<1))>0  ) )
{
    L_sum1=L_SATURATE(s_lo);//
}
else if  (( ((s_lo<<1)&0x80000000)) && ( ((INT32)(s_lo))< LW_MIN  ) )
{
    L_sum1=L_SATURATE((INT32)(s_lo));
}
else
{
    if ( ((INT32)(s_lo))< 0  )
    {
      L_sum1 = VPP_SCALE64_TO_16(s_hi, s_lo);
    }
    else
    {
      tempL = VPP_SCALE64_TO_16(s_hi, s_lo);
      L_sum1=L_SATURATE(tempL);
    }
}*/

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


#   define VPP_SCALE64_TO_16(hi, lo)  (lo << 1)

#   define VPP_MULT_R(hi, lo, x, y)\
     ({ lo = 0x00004000L;  \
        VPP_MLA16(hi,lo,x, y);\
    })

# define L_MLA_CARRY_SAT(hi, lo, carry)\
    ((hi ==  0) && ((lo & 0xc0000000) ==  0)) ? ((lo << 1) + (carry)) : \
    ((hi == -1) && ((lo | 0x3fffffff) == -1)) ? ((lo << 1) + (carry)) : \
     (hi >=  0)                             ? LW_MAX : \
                                              LW_MIN


#   define L_MLA_SAT(hi, lo)\
    ((lo>=LW_MAX) && (((INT32)(lo<<1))>0))?L_SATURATE(lo):\
    ( (( ((lo<<1)&0x80000000)) && (((INT32)(lo))< LW_MIN))?L_SATURATE((INT32)(lo)):\
      ( ((INT32)(lo))< 0  )?VPP_SCALE64_TO_16(hi, lo): L_SATURATE( (UINT32)VPP_SCALE64_TO_16(hi, lo)) )



#endif






























#endif // VPP_HR_MATHHALF_MACRO_H_INCLUDED
