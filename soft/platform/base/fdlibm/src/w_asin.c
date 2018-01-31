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
double asin(double x)       /* wrapper asin */
#else
double asin(x)          /* wrapper asin */
double x;
#endif
{
#ifdef _IEEE_LIBM
    return __ieee754_asin(x);
#else
    double z;
    z = __ieee754_asin(x);
    if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
    if(fabs(x)>1.0)
    {
        return __kernel_standard(x,x,2); /* asin(|x|>1) */
    }
    else
        return z;
#endif
}
