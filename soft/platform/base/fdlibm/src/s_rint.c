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
TWO52[2]=
{
    4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
    -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

#ifdef __STDC__
double rint(double x)
#else
double rint(x)
double x;
#endif
{
    int32_t i0,j0,sx;
    uint32_t i,i1;
    double t;
    volatile double w;
    EXTRACT_WORDS(i0,i1,x);
    sx = (i0>>31)&1;
    j0 = ((i0>>20)&0x7ff)-0x3ff;
    if(j0<20)
    {
        if(j0<0)
        {
            if(((i0&0x7fffffff)|i1)==0) return x;
            i1 |= (i0&0x0fffff);
            i0 &= 0xfffe0000;
            i0 |= ((i1|-i1)>>12)&0x80000;
            SET_HIGH_WORD(x,i0);
            w = TWO52[sx]+x;
            t =  w-TWO52[sx];
            GET_HIGH_WORD(i0,t);
            SET_HIGH_WORD(t,(i0&0x7fffffff)|(sx<<31));
            return t;
        }
        else
        {
            i = (0x000fffff)>>j0;
            if(((i0&i)|i1)==0) return x; /* x is integral */
            i>>=1;
            if(((i0&i)|i1)!=0)
            {
                if(j0==19) i1 = 0x40000000; else
                    i0 = (i0&(~i))|((0x20000)>>j0);
            }
        }
    }
    else if (j0>51)
    {
        if(j0==0x400) return x+x;   /* inf or NaN */
        else return x;      /* x is integral */
    }
    else
    {
        i = ((uint32_t)(0xffffffff))>>(j0-20);
        if((i1&i)==0) return x; /* x is integral */
        i>>=1;
        if((i1&i)!=0) i1 = (i1&(~i))|((0x40000000)>>(j0-20));
    }
    INSERT_WORDS(x,i0,i1);
    w = TWO52[sx]+x;
    return w-TWO52[sx];
}
#endif /* _DOUBLE_IS_32BITS */
