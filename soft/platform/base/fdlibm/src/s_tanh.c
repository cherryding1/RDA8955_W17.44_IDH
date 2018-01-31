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
static const double one=1.0, two=2.0, tiny = 1.0e-300;
#else
static double one=1.0, two=2.0, tiny = 1.0e-300;
#endif

#ifdef __STDC__
double tanh(double x)
#else
double tanh(x)
double x;
#endif
{
    double t,z;
    int32_t jx,ix;

    /* High word of |x|. */
    GET_HIGH_WORD(jx,x);
    ix = jx&0x7fffffff;

    /* x is INF or NaN */
    if(ix>=0x7ff00000)
    {
        if (jx>=0) return one/x+one;    /* tanh(+-inf)=+-1 */
        else       return one/x-one;    /* tanh(NaN) = NaN */
    }

    /* |x| < 22 */
    if (ix < 0x40360000)        /* |x|<22 */
    {
        if (ix<0x3c800000)      /* |x|<2**-55 */
            return x*(one+x);       /* tanh(small) = small */
        if (ix>=0x3ff00000)     /* |x|>=1  */
        {
            t = expm1(two*fabs(x));
            z = one - two/(t+two);
        }
        else
        {
            t = expm1(-two*fabs(x));
            z= -t/(t+two);
        }
        /* |x| > 22, return +-1 */
    }
    else
    {
        z = one - tiny;     /* raised inexact flag */
    }
    return (jx>=0)? z: -z;
}
#endif /* _DOUBLE_IS_32BITS */
