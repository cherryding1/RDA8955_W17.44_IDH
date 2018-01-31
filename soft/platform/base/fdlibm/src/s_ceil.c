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
static const double huge = 1.0e300;
#else
static double huge = 1.0e300;
#endif

#ifdef __STDC__
double ceil(double x)
#else
double ceil(x)
double x;
#endif
{
    int32_t i0,i1,j0;
    uint32_t i,j;
    EXTRACT_WORDS(i0,i1,x);
    j0 = ((i0>>20)&0x7ff)-0x3ff;
    if(j0<20)
    {
        if(j0<0)    /* raise inexact if x != 0 */
        {
            if(huge+x>0.0)  /* return 0*sign(x) if |x|<1 */
            {
                if(i0<0) {i0=0x80000000; i1=0;}
                else if((i0|i1)!=0) { i0=0x3ff00000; i1=0;}
            }
        }
        else
        {
            i = (0x000fffff)>>j0;
            if(((i0&i)|i1)==0) return x; /* x is integral */
            if(huge+x>0.0)      /* raise inexact flag */
            {
                if(i0>0) i0 += (0x00100000)>>j0;
                i0 &= (~i); i1=0;
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
        if(huge+x>0.0)          /* raise inexact flag */
        {
            if(i0>0)
            {
                if(j0==20) i0+=1;
                else
                {
                    j = i1 + (1<<(52-j0));
                    if(j<i1) i0+=1; /* got a carry */
                    i1 = j;
                }
            }
            i1 &= (~i);
        }
    }
    INSERT_WORDS(x,i0,i1);
    return x;
}

#endif
