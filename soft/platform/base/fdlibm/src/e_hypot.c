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
double __ieee754_hypot(double x, double y)
#else
double __ieee754_hypot(x,y)
double x, y;
#endif
{
    double a=x,b=y,t1,t2,y1,y2,w;
    uint32_t j,k,ha,hb,hx,hy;

    GET_HIGH_WORD(hx,x);
    GET_HIGH_WORD(hy,y);
    ha = hx&0x7fffffff; /* high word of  x */
    hb = hy&0x7fffffff; /* high word of  y */
    if(hb > ha) {a=y; b=x; j=ha; ha=hb; hb=j;}
    else {a=x; b=y;}
    SET_HIGH_WORD(a,ha); /* a <- |a| */
    SET_HIGH_WORD(b,hb); /* b <- |b| */
    if((ha-hb)>0x3c00000) {return a+b;} /* x/y > 2**60 */
    k=0;
    if(ha > 0x5f300000)     /* a>2**500 */
    {
        if(ha >= 0x7ff00000)     /* Inf or NaN */
        {
            uint32_t la, lb;
            w = a+b;         /* for sNaN */
            GET_LOW_WORD(la,a);
            GET_LOW_WORD(lb,b);
            if(((ha&0xfffff)|la)==0) w = a;
            if(((hb^0x7ff00000)|lb)==0) w = b;
            return w;
        }
        /* scale a and b by 2**-600 */
        ha -= 0x25800000; hb -= 0x25800000;  k += 600;
        SET_HIGH_WORD(a,ha);
        SET_HIGH_WORD(b,hb);
    }
    if(hb < 0x20b00000)     /* b < 2**-500 */
    {
        if(hb <= 0x000fffff)    /* subnormal b or 0 */
        {
            uint32_t lb;
            GET_LOW_WORD(lb,b);
            if((hb|lb)==0) return a;
            t1=0;
            SET_HIGH_WORD(t1, 0x7fd00000);  /* t1=2^1022 */
            b *= t1;
            a *= t1;
            k -= 1022;
        }
        else            /* scale a and b by 2^600 */
        {
            ha += 0x25800000;   /* a *= 2^600 */
            hb += 0x25800000;   /* b *= 2^600 */
            k -= 600;

            SET_HIGH_WORD(a,ha);
            SET_HIGH_WORD(b,hb);
        }
    }
    /* medium size a and b */
    w = a-b;
    if (w>b)
    {
        t1 = 0;
        SET_HIGH_WORD(t1, ha);
        t2 = a-t1;
        w  = sqrt(t1*t1-(b*(-b)-t2*(a+t1)));
    }
    else
    {
        a  = a+a;
        y1 = 0;
        SET_HIGH_WORD(y1, hb);
        y2 = b - y1;
        t1 = 0;
        SET_HIGH_WORD(t1, ha+0x00100000);
        t2 = a - t1;
        w  = sqrt(t1*y1-(w*(-w)-(t1*y2+t2*b)));
    }
    if(k!=0)
    {
        uint32_t ht1;

        t1 = 1.0;
        GET_HIGH_WORD(ht1, t1);
        SET_HIGH_WORD(t1, ht1 + (k<<20));
        return t1*w;
    }
    else return w;
}
#endif /* defined(_DOUBLE_IS_32BITS) */
