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





#ifndef _ASSEMBLY_H
#define _ASSEMBLY_H

#ifdef SHEEN_VC_DEBUG

typedef __int64 Word64;
#define __inline__ __inline

#else
#include "mcip_debug.h"
typedef long long Word64;

#endif

static __inline__ int MULSHIFT32(int x, int y)
{
    int z;

    z = (Word64)x * (Word64)y >> 32;

    return z;
}

static __inline int FASTABS(int x)
{
    int sign;

    sign = x >> (sizeof(int) * 8 - 1);
    x ^= sign;
    x -= sign;

    return x;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return (sizeof(int) * 8);

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

/* MADD64, SHL64, SAR64:
 * write in assembly to avoid dependency on run-time lib for 64-bit shifts, muls
 *  (sometimes compiler thunks to function calls instead of code generating)
 * required for Symbian emulator
 */
typedef union _U64
{
    Word64 w64;
    struct
    {
        /* PowerPC = big endian */
        unsigned int lo32;
        signed int   hi32;
    } r;
} U64MP3;


static __inline Word64 MADD64(Word64 sum64_in, int x, int y)
{
#ifdef SHEEN_VC_DEBUG

    sum64_in += (Word64)x * (Word64)y;

    return sum64_in;

#else //sheen
    U64MP3 sum64;
    sum64.w64=sum64_in;
    //sum64_in += (Word64)x * (Word64)y;

    //return sum64;

    //  asm ("madd    %2,%3"   : "=l" (lo), "=h" (hi)  : "%r" (x), "r" (y))


    //asm ("madd  %2,%3"   :  : "%r" (x), "r" (y));
    //asm ("madd %2,%3"   :  : "%r" (x), "r" (y));
    //asm ("madd %2,%3"   :  : "%r" (x), "r" (y));
    //asm ("madd %2,%3"   :  : "%r" (x), "r" (y));
    //asm ("madd %2,%3"   :  : "%r" (x), "r" (y));
    //asm("mtlo %0" : : "r"(sum64.r.lo32));
//   asm("mthi %0" : : "r"(sum64.r.hi32));

    //asm ("madd %2,%3"   : "+l" (sum64.r.lo32), "+h" (sum64.r.hi32)  : "r" (x), "r" (y));
//     asm ("madd        %2,%3\t\nmflo %0\t\nmfhi %1" : "=r" (sum64.r.lo32), "=r" (sum64.r.hi32) : "r" (x), "r" (y));
    asm ("mtlo %4\t\nmthi %5\t\nmadd %2,%3\t\nmflo %0\t\nmfhi %1" : "=r" (sum64.r.lo32), "=r" (sum64.r.hi32) : "r" (x), "r" (y),"r" (sum64.r.lo32), "r" (sum64.r.hi32));

    //if(sum64.w64 != sum64_in)
    //{
    //  diag_printf("%d, %d,%d,%d \n", x,y,sum64.r.lo32, sum64.r.hi32);
    //  diag_printf("%d, %d, %d\n",  (unsigned int)sum64_in, (int)(sum64_in>>32),temp);
    //}
    return sum64.w64;
    //return sum64_in;
#endif

}

static __inline Word64 SHL64(Word64 x, int n)
{
    return x<<n;
}

static __inline Word64 SAR64(Word64 x, int n)
{
    return x>>n;
}



#endif /* _ASSEMBLY_H */
