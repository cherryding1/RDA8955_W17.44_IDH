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
static const double
#else
static double
#endif
one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
C1  =  4.16666666666666019037e-02, /* 0x3FA55555, 0x5555554C */
C2  = -1.38888888888741095749e-03, /* 0xBF56C16C, 0x16C15177 */
C3  =  2.48015872894767294178e-05, /* 0x3EFA01A0, 0x19CB1590 */
C4  = -2.75573143513906633035e-07, /* 0xBE927E4F, 0x809C52AD */
C5  =  2.08757232129817482790e-09, /* 0x3E21EE9E, 0xBDB4B1C4 */
C6  = -1.13596475577881948265e-11; /* 0xBDA8FAE9, 0xBE8838D4 */

#ifdef __STDC__
double __kernel_cos(double x, double y)
#else
double __kernel_cos(x, y)
double x,y;
#endif
{
    double a,hz,z,r,qx;
    int32_t ix;
    GET_HIGH_WORD(ix, x);
    ix &= 0x7fffffff;   /* ix = |x|'s high word*/
    if(ix<0x3e400000)           /* if x < 2**27 */
    {
        if(((int)x)==0) return one;     /* generate inexact */
    }
    z  = x*x;
    r  = z*(C1+z*(C2+z*(C3+z*(C4+z*(C5+z*C6)))));
    if(ix < 0x3FD33333)             /* if |x| < 0.3 */
        return one - (0.5*z - (z*r - x*y));
    else
    {
        if(ix > 0x3fe90000)         /* x > 0.78125 */
        {
            qx = 0.28125;
        }
        else
        {
            INSERT_WORDS(qx,ix-0x00200000,0);
        }
        hz = 0.5*z-qx;
        a  = one-qx;
        return a - (hz - (z*r-x*y));
    }
}
#endif /* defined(_DOUBLE_IS_32BITS) */
