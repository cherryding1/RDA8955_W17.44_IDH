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

#ifdef __STDC__
static const float
#else
static float
#endif
TWO23[2]=
{
    8.3886080000e+06, /* 0x4b000000 */
    -8.3886080000e+06, /* 0xcb000000 */
};

#ifdef __STDC__
float rintf(float x)
#else
float rintf(x)
float x;
#endif
{
    int32_t i0,j0,sx;
    uint32_t i,i1;
    float w,t;
    GET_FLOAT_WORD(i0,x);
    sx = (i0>>31)&1;
    j0 = ((i0>>23)&0xff)-0x7f;
    if(j0<23)
    {
        if(j0<0)
        {
            if((i0&0x7fffffff)==0) return x;
            i1 = (i0&0x07fffff);
            i0 &= 0xfff00000;
            i0 |= ((i1|-i1)>>9)&0x400000;
            SET_FLOAT_WORD(x,i0);
            w = TWO23[sx]+x;
            t =  w-TWO23[sx];
            GET_FLOAT_WORD(i0,t);
            SET_FLOAT_WORD(t,(i0&0x7fffffff)|(sx<<31));
            return t;
        }
        else
        {
            i = (0x007fffff)>>j0;
            if((i0&i)==0) return x; /* x is integral */
            i>>=1;
            if((i0&i)!=0) i0 = (i0&(~i))|((0x100000)>>j0);
        }
    }
    else
    {
        if(j0==0x80) return x+x;    /* inf or NaN */
        else return x;      /* x is integral */
    }
    SET_FLOAT_WORD(x,i0);
    w = TWO23[sx]+x;
    return w-TWO23[sx];
}

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
double rint(double x)
#else
double rint(x)
double x;
#endif
{
    return (double) rintf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
