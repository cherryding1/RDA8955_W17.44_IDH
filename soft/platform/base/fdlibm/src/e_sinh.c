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
static const double one = 1.0, shuge = 1.0e307;
#else
static double one = 1.0, shuge = 1.0e307;
#endif

#ifdef __STDC__
double __ieee754_sinh(double x)
#else
double __ieee754_sinh(x)
double x;
#endif
{
    double t,w,h;
    int32_t ix,jx;
    uint32_t lx;

    /* High word of |x|. */
    GET_HIGH_WORD(jx,x);
    ix = jx&0x7fffffff;

    /* x is INF or NaN */
    if(ix>=0x7ff00000) return x+x;

    h = 0.5;
    if (jx<0) h = -h;
    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x40360000)        /* |x|<22 */
    {
        if (ix<0x3e300000)      /* |x|<2**-28 */
            if(shuge+x>one) return x;/* sinh(tiny) = tiny with inexact */
        t = expm1(fabs(x));
        if(ix<0x3ff00000) return h*(2.0*t-t*t/(t+one));
        return h*(t+t/(t+one));
    }

    /* |x| in [22, log(maxdouble)] return 0.5*exp(|x|) */
    if (ix < 0x40862E42)  return h*__ieee754_exp(fabs(x));

    /* |x| in [log(maxdouble), overflowthresold] */
    lx = *( (((*(uint32_t*)&one)>>29)) + (uint32_t*)&x);
    if (ix<0x408633CE || (ix==0x408633ce)&&(lx<=(uint32_t)0x8fb9f87d))
    {
        w = __ieee754_exp(0.5*fabs(x));
        t = h*w;
        return t*w;
    }

    /* |x| > overflowthresold, sinh(x) overflow */
    return x*shuge;
}
#endif /* defined(_DOUBLE_IS_32BITS) */
