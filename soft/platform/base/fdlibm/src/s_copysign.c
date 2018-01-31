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
double copysign(double x, double y)
#else
double copysign(x,y)
double x,y;
#endif
{
    uint32_t hx, hy;
    GET_HIGH_WORD(hx, x);
    GET_HIGH_WORD(hy, y);
    SET_HIGH_WORD(x, (hx&0x7fffffff)|(hy&0x80000000));
    return x;
}
#endif /* _DOUBLE_IS_32BITS */
