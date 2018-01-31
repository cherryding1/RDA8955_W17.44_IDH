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

#ifdef _SCALB_INT
#ifdef __STDC__
double __ieee754_scalb(double x, int fn)
#else
double __ieee754_scalb(x,fn)
double x; int fn;
#endif
#else
#ifdef __STDC__
double __ieee754_scalb(double x, double fn)
#else
double __ieee754_scalb(x,fn)
double x, fn;
#endif
#endif
{
#ifdef _SCALB_INT
    return scalbn(x,fn);
#else
    if (isnan(x)||isnan(fn)) return x*fn;
    if (!finite(fn))
    {
        if(fn>0.0) return x*fn;
        else       return x/(-fn);
    }
    if (rint(fn)!=fn) return (fn-fn)/(fn-fn);
    if ( fn > 65000.0) return scalbn(x, 65000);
    if (-fn > 65000.0) return scalbn(x,-65000);
    return scalbn(x,(int)fn);
#endif
}
