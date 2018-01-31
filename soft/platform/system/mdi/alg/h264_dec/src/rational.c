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








#include "common.h"
//#include "mathematics.h"
#include "rational.h"
#ifdef SHEEN_VC_DEBUG
#include <limits.h>
#include <assert.h>
#endif

int av_reduce(int *dst_nom, int *dst_den, int64_t nom, int64_t den, int64_t max)
{
    AVRational a0= {0,1}, a1= {1,0};
    int sign= (nom<0) ^ (den<0);
    int64_t gcd= ff_gcd(FFABS(nom), FFABS(den));

    if(gcd)
    {
        nom = FFABS(nom)/gcd;
        den = FFABS(den)/gcd;
    }
    if(nom<=max && den<=max)
    {
        a1. num = nom; ///< numerator
        a1.den=den; ///< denominator

        den=0;
    }

    while(den)
    {
        uint64_t x      = nom / den;
        int64_t next_den= nom - den*x;
        int64_t a2n= x*a1.num + a0.num;
        int64_t a2d= x*a1.den + a0.den;

        if(a2n > max || a2d > max)
        {
            if(a1.num) x= (max - a0.num) / a1.num;
            if(a1.den) x= FFMIN(x, (max - a0.den) / a1.den);

            if (den*(2*x*a1.den + a0.den) > nom*a1.den)
//                a1 = (AVRational){x*a1.num + a0.num, x*a1.den + a0.den};
                break;
        }

        a0= a1;
//       a1= (AVRational){a2n, a2d};
        nom= den;
        den= next_den;
    }
    AV_ASSERT(ff_gcd(a1.num, a1.den) <= 1U);

    *dst_nom = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den==0;
}

#ifdef SHEEN_VC_DEBUG
AVRational av_mul_q(AVRational b, AVRational c)
{
    av_reduce(&b.num, &b.den, b.num * (int64_t)c.num, b.den * (int64_t)c.den, (int64_t)INT_MAX);
    return b;
}
static AVRational AV00 = {0,0};
AVRational av_div_q(AVRational b, AVRational c)
{
    // return av_mul_q(b, (AVRational){c.den, c.num});
    return AV00;
}

AVRational av_add_q(AVRational b, AVRational c)
{
    av_reduce(&b.num, &b.den, b.num * (int64_t)c.den + c.num * (int64_t)b.den, b.den * (int64_t)c.den, (int64_t)INT_MAX);
    return b;
}

AVRational av_sub_q(AVRational b, AVRational c)
{
//    return av_add_q(b, (AVRational){-c.num, c.den});
    return AV00;
}


AVRational av_d2q(double d, int max_val)
{
    AVRational a;
#define LOG2  0.69314718055994530941723212145817656807550013436025
    int exponent= FFMAX( (int)(log(fabs(d) + 1e-20)/LOG2), 0);
    int64_t den= 1 << (61 - exponent);
    av_reduce(&a.num, &a.den, (int64_t)(d * den + 0.5), den, max_val);

    return a;
}
#endif
