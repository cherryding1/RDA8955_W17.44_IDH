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







#include "fdlibm.h"

#ifndef _DOUBLE_IS_32BITS

#ifdef __STDC__
static const double
#else
static double
#endif
one = 1.0,
halF[2] = {0.5,-0.5,},
          huge    = 1.0e+300,
          twom1000= 9.33263618503218878990e-302,     /* 2**-1000=0x01700000,0*/
          o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
          u_threshold= -7.45133219101941108420e+02,  /* 0xc0874910, 0xD52D3051 */
          ln2HI[2]   = { 6.93147180369123816490e-01, /* 0x3fe62e42, 0xfee00000 */
                         -6.93147180369123816490e-01,
                       },/* 0xbfe62e42, 0xfee00000 */
                       ln2LO[2]   = { 1.90821492927058770002e-10, /* 0x3dea39ef, 0x35793c76 */
                                      -1.90821492927058770002e-10,
                                    },/* 0xbdea39ef, 0x35793c76 */
                                    invln2 =  1.44269504088896338700e+00, /* 0x3ff71547, 0x652b82fe */
                                    P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
                                    P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
                                    P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
                                    P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
                                    P5   =  4.13813679705723846039e-08; /* 0x3E663769, 0x72BEA4D0 */


#ifdef __STDC__
double __ieee754_exp(double x)  /* default IEEE double exp */
#else
double __ieee754_exp(x) /* default IEEE double exp */
double x;
#endif
{
    double y,hi,lo,c,t;
    int32_t k,xsb;
    uint32_t hx;

    GET_HIGH_WORD(hx,x);    /* high word of x */
    xsb = (hx>>31)&1;       /* sign bit of x */
    hx &= 0x7fffffff;       /* high word of |x| */

    /* filter out non-finite argument */
    if(hx >= 0x40862E42)            /* if |x|>=709.78... */
    {
        if(hx>=0x7ff00000)
        {
            uint32_t lx;
            GET_LOW_WORD(lx,x);
            if(((hx&0xfffff)|lx)!=0)
                return x+x;        /* NaN */
            else return (xsb==0)? x:0.0;    /* exp(+-inf)={inf,0} */
        }
        if(x > o_threshold) return huge*huge; /* overflow */
        if(x < u_threshold) return twom1000*twom1000; /* underflow */
    }

    /* argument reduction */
    if(hx > 0x3fd62e42)         /* if  |x| > 0.5 ln2 */
    {
        if(hx < 0x3FF0A2B2)     /* and |x| < 1.5 ln2 */
        {
            hi = x-ln2HI[xsb]; lo=ln2LO[xsb]; k = 1-xsb-xsb;
        }
        else
        {
            k  = (int32_t)(invln2*x+halF[xsb]);
            t  = k;
            hi = x - t*ln2HI[0];    /* t*ln2HI is exact here */
            lo = t*ln2LO[0];
        }
        x  = hi - lo;
    }
    else if(hx < 0x3e300000)    /* when |x|<2**-28 */
    {
        if(huge+x>one) return one+x;/* trigger inexact */
    }
    else k = 0;

    /* x is now in primary range */
    t  = x*x;
    c  = x - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
    if(k==0)    return one-((x*c)/(c-2.0)-x);
    else        y = one-((lo-(x*c)/(2.0-c))-hi);
    if(k >= -1021)
    {
        uint32_t hy;
        GET_HIGH_WORD(hy, y);
        SET_HIGH_WORD(y, hy + (k<<20)); /* add k to y's exponent */
        return y;
    }
    else
    {
        uint32_t hy;
        GET_HIGH_WORD(hy, y);
        SET_HIGH_WORD(y, hy + ((k+1000)<<20));/* add k to y's exponent */
        return y*twom1000;
    }
}
#endif /* defined(_DOUBLE_IS_32BITS) */
