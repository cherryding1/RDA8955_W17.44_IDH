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
static const double
#else
static double
#endif
o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
u_threshold= -7.45133219101941108420e+02;  /* 0xc0874910, 0xD52D3051 */

#ifdef __STDC__
double exp(double x)        /* wrapper exp */
#else
double exp(x)           /* wrapper exp */
double x;
#endif
{
#ifdef _IEEE_LIBM
    return __ieee754_exp(x);
#else
    double z;
    z = __ieee754_exp(x);
    if(_LIB_VERSION == _IEEE_) return z;
    if(finite(x))
    {
        if(x>o_threshold)
            return __kernel_standard(x,x,6); /* exp overflow */
        else if(x<u_threshold)
            return __kernel_standard(x,x,7); /* exp underflow */
    }
    return z;
#endif
}
