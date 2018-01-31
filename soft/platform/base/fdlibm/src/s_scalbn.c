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
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
huge   = 1.0e+300,
tiny   = 1.0e-300;

#ifdef __STDC__
double scalbn (double x, int n)
#else
double scalbn (x,n)
double x; int n;
#endif
{
    int32_t  k,hx,lx;
    EXTRACT_WORDS(hx,lx,x);
    k = (hx&0x7ff00000)>>20;        /* extract exponent */
    if (k==0)               /* 0 or subnormal x */
    {
        if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
        x *= two54;
        GET_HIGH_WORD(hx,x);
        k = ((hx&0x7ff00000)>>20) - 54;
        if (n< -50000) return tiny*x;   /*underflow*/
    }
    if (k==0x7ff) return x+x;       /* NaN or Inf */
    k = k+n;
    if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
    if (k > 0)              /* normal result */
    {SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20)); return x;}
    if (k <= -54)
        if (n > 50000)  /* in case integer overflow in n+k */
            return huge*copysign(huge,x);   /*overflow*/
        else return tiny*copysign(tiny,x);  /*underflow*/
    k += 54;                /* subnormal result */
    SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));
    return x*twom54;
}
#endif /* _DOUBLE_IS_32BITS */
