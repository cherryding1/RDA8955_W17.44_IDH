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
static const double zero = 0.0;
#else
static double zero = 0.0;
#endif


#ifdef __STDC__
double __ieee754_remainder(double x, double p)
#else
double __ieee754_remainder(x,p)
double x,p;
#endif
{
    int32_t hx,hp;
    uint32_t sx,lx,lp;
    double p_half;

    EXTRACT_WORDS(hx,lx,x);
    EXTRACT_WORDS(hp,lp,p);
    sx = hx&0x80000000;
    hp &= 0x7fffffff;
    hx &= 0x7fffffff;

    /* purge off exception values */
    if((hp|lp)==0) return (x*p)/(x*p);  /* p = 0 */
    if((hx>=0x7ff00000)||           /* x not finite */
            ((hp>=0x7ff00000)&&           /* p is NaN */
             (((hp-0x7ff00000)|lp)!=0)))
        return (x*p)/(x*p);


    if (hp<=0x7fdfffff) x = __ieee754_fmod(x,p+p);  /* now x < 2p */
    if (((hx-hp)|(lx-lp))==0) return zero*x;
    x  = fabs(x);
    p  = fabs(p);
    if (hp<0x00200000)
    {
        if(x+x>p)
        {
            x-=p;
            if(x+x>=p) x -= p;
        }
    }
    else
    {
        p_half = 0.5*p;
        if(x>p_half)
        {
            x-=p;
            if(x>=p_half) x -= p;
        }
    }
    GET_HIGH_WORD(hx,x);
    SET_HIGH_WORD(x,hx ^ sx);
    return x;
}
#endif /* defined(_DOUBLE_IS_32BITS) */
