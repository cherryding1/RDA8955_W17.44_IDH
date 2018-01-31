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
double cosh(double x)       /* wrapper cosh */
#else
double cosh(x)          /* wrapper cosh */
double x;
#endif
{
#ifdef _IEEE_LIBM
    return __ieee754_cosh(x);
#else
    double z;
    z = __ieee754_cosh(x);
    if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
    if(fabs(x)>7.10475860073943863426e+02)
    {
        return __kernel_standard(x,x,5); /* cosh overflow */
    }
    else
        return z;
#endif
}
