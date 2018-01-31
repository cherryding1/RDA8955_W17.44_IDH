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
double pow(double x, double y)  /* wrapper pow */
#else
double pow(x,y)         /* wrapper pow */
double x,y;
#endif
{
#ifdef _IEEE_LIBM
    return  __ieee754_pow(x,y);
#else
    double z;
    z=__ieee754_pow(x,y);
    if(_LIB_VERSION == _IEEE_|| isnan(y)) return z;
    if(isnan(x))
    {
        if(y==0.0)
            return __kernel_standard(x,y,42); /* pow(NaN,0.0) */
        else
            return z;
    }
    if(x==0.0)
    {
        if(y==0.0)
            return __kernel_standard(x,y,20); /* pow(0.0,0.0) */
        if(finite(y)&&y<0.0)
            return __kernel_standard(x,y,23); /* pow(0.0,negative) */
        return z;
    }
    if(!finite(z))
    {
        if(finite(x)&&finite(y))
        {
            if(isnan(z))
                return __kernel_standard(x,y,24); /* pow neg**non-int */
            else
                return __kernel_standard(x,y,21); /* pow overflow */
        }
    }
    if(z==0.0&&finite(x)&&finite(y))
        return __kernel_standard(x,y,22); /* pow underflow */
    return z;
#endif
}
