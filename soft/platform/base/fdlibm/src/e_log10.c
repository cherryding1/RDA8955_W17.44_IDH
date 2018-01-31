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
two54      =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
ivln10     =  4.34294481903251816668e-01, /* 0x3FDBCB7B, 0x1526E50E */
log10_2hi  =  3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo  =  3.69423907715893078616e-13; /* 0x3D59FEF3, 0x11F12B36 */

static double zero   =  0.0;

#ifdef __STDC__
double __ieee754_log10(double x)
#else
double __ieee754_log10(x)
double x;
#endif
{
    double y,z;
    int32_t i,k,hx;
    uint32_t lx;

    EXTRACT_WORDS(hx,lx,x);

    k=0;
    if (hx < 0x00100000)                    /* x < 2**-1022  */
    {
        if (((hx&0x7fffffff)|lx)==0)
            return -two54/zero;             /* log(+-0)=-inf */
        if (hx<0) return (x-x)/zero;        /* log(-#) = NaN */
        k -= 54; x *= two54; /* subnormal number, scale up x */
        GET_HIGH_WORD(hx, x);              /* high word of x */
    }
    if (hx >= 0x7ff00000) return x+x;
    k += (hx>>20)-1023;
    i  = ((uint32_t)k&0x80000000)>>31;
    hx = (hx&0x000fffff)|((0x3ff-i)<<20);
    y  = (double)(k+i);
    SET_HIGH_WORD(x,hx);
    z  = y*log10_2lo + ivln10*__ieee754_log(x);
    return  z+y*log10_2hi;
}
#endif /* defined(_DOUBLE_IS_32BITS) */
