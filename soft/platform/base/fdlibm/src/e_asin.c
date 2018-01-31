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
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
huge =  1.000e+300,
pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
pio4_hi =  7.85398163397448278999e-01, /* 0x3FE921FB, 0x54442D18 */
/* coefficient for R(x^2) */
pS0 =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
pS2 =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
pS4 =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
pS5 =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
qS2 =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

#ifdef __STDC__
double __ieee754_asin(double x)
#else
double __ieee754_asin(x)
double x;
#endif
{
    double t,w,p,q,c,r,s;
    int32_t hx,ix;
    GET_HIGH_WORD(hx,x);
    ix = hx&0x7fffffff;
    if(ix>= 0x3ff00000)         /* |x|>= 1 */
    {
        uint32_t lx;
        GET_LOW_WORD(lx,x);
        if(((ix-0x3ff00000)|lx)==0)
            /* asin(1)=+-pi/2 with inexact */
            return x*pio2_hi+x*pio2_lo;
        return (x-x)/(x-x);     /* asin(|x|>1) is NaN */
    }
    else if (ix<0x3fe00000)     /* |x|<0.5 */
    {
        if(ix<0x3e400000)       /* if |x| < 2**-27 */
        {
            if(huge+x>one) return x;/* return x with inexact if x!=0*/
        }
        else
            t = x*x;
        p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
        q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
        w = p/q;
        return x+x*w;
    }
    /* 1> |x|>= 0.5 */
    w = one-fabs(x);
    t = w*0.5;
    p = t*(pS0+t*(pS1+t*(pS2+t*(pS3+t*(pS4+t*pS5)))));
    q = one+t*(qS1+t*(qS2+t*(qS3+t*qS4)));
    s = __ieee754_sqrt(t);
    if(ix>=0x3FEF3333)      /* if |x| > 0.975 */
    {
        w = p/q;
        t = pio2_hi-(2.0*(s+s*w)-pio2_lo);
    }
    else
    {
        w  = s;
        SET_LOW_WORD(w,0);
        c  = (t-w*w)/(s+w);
        r  = p/q;
        p  = 2.0*s*r-(pio2_lo-2.0*c);
        q  = pio4_hi-2.0*w;
        t  = pio4_hi-(p-q);
    }
    if(hx>0) return t; else return -t;
}
#endif /* defined(_DOUBLE_IS_32BITS) */
