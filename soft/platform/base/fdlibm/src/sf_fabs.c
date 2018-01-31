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
float fabsf(float x)
#else
float fabsf(x)
float x;
#endif
{
    uint32_t ix;
    GET_FLOAT_WORD(ix,x);
    SET_FLOAT_WORD(x,ix&0x7fffffff);
    return x;
}

#ifdef _DOUBLE_IS_32BITS

#ifdef __STDC__
double fabs(double x)
#else
double fabs(x)
double x;
#endif
{
    return (double) fabsf((float) x);
}

#endif /* defined(_DOUBLE_IS_32BITS) */
