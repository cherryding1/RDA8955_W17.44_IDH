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
int finite(double x)
#else
int finite(x)
double x;
#endif
{
    uint32_t hx;
    GET_HIGH_WORD(hx,x);
    return  (unsigned)((hx&0x7fffffff)-0x7ff00000)>>31;
}
