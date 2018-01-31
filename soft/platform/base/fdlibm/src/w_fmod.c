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
double fmod(double x, double y) /* wrapper fmod */
#else
double fmod(x,y)        /* wrapper fmod */
double x,y;
#endif
{
#ifdef _IEEE_LIBM
    return __ieee754_fmod(x,y);
#else
    double z;
    z = __ieee754_fmod(x,y);
    if(_LIB_VERSION == _IEEE_ ||isnan(y)||isnan(x)) return z;
    if(y==0.0)
    {
        return __kernel_standard(x,y,27); /* fmod(x,0) */
    }
    else
        return z;
#endif
}
