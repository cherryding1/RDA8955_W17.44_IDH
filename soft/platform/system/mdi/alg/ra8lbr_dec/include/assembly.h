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

#include "regdef.h"
#include "coder.h"

/* toolchain:           MSFT Visual C++
 * target architecture: x86
 */
#if (defined _WIN32 && !defined _WIN32_WCE) || (defined __WINS__ && defined _SYMBIAN) || (defined WINCE_EMULATOR) || (defined _OPENWAVE_SIMULATOR)

#pragma warning( disable : 4035 )   /* complains about inline asm not returning a value */

static __inline int MULSHIFT32(int x, int y)
{
    __asm
    {
        mov     eax, x
        imul    y
        mov     eax, edx
    }
}

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
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

/* toolchain:           MSFT Embedded Visual C++
 * target architecture: ARM v.4 and above (require 'M' type processor for 32x32->64 multiplier)
 */
#elif defined _WIN32 && defined _WIN32_WCE && defined ARM

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
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

/* implemented in asmfunc.s */
#ifdef __cplusplus
extern "C" {
#endif
int MULSHIFT32(int x, int y);
#ifdef __cplusplus
}
#endif

/* toolchain:           ARM ADS or RealView
 * target architecture: ARM v.4 and above (requires 'M' type processor for 32x32->64 multiplier)
 */
#elif defined ARM_ADS

static __inline int MULSHIFT32(int x, int y)
{
    /* rules for smull RdLo, RdHi, Rm, Rs:
     *   RdHi != Rm
     *   RdLo != Rm
     *   RdHi != RdLo
     */
    int zlow;
    __asm
    {
        smull zlow,y,x,y
    }

    return y;
}

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
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

/* toolchain:           ARM gcc
 * target architecture: ARM v.4 and above (requires 'M' type processor for 32x32->64 multiplier)
 */
#elif defined(__GNUC__) && defined(ARM)

static __inline__ int MULSHIFT32(int x, int y)
{
    int zlow;
    __asm__ volatile ("smull %0,%1,%2,%3" : "=&r" (zlow), "=r" (y) : "r" (x), "1" (y));
    return y;
}

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
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

/* toolchain:           x86 gcc
 * target architecture: x86
 */
#elif defined(__GNUC__) && defined(__i386__)

static __inline__ int MULSHIFT32(int x, int y)
{
    int z;
    __asm__ volatile ("imull %3" : "=d" (z), "=a" (x): "1" (x), "r" (y));
    return z;
}

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
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

#else

#define MULSHIFT32hx(x, y, z) \
    z=((long long)(x)*(long long)(y))>>32
//asm volatile("mult %1,%2\n\tmfhi %0\n\t":"=r"(z):"r"(x),"r"(y))


static __inline int MULSHIFT32(int x, int y)
{
    long long res;
    res=((long long)x*(long long)y)>>32;

    return res;
}

static __inline short CLIPTOSHORT(int x)
{
    int sign;

    /* clip to [-32768, 32767] */
    sign = x >> 31;
    if (sign != (x >> 15))
        x = sign ^ ((1 << 15) - 1);

    return (short)x;
}

static __inline int FASTABS(int x)
{
    int sign;

    sign = x >> 31;
    x ^= sign;
    x -= sign;

    return x;
}

static __inline int CLZ(int x)
{
    int numZeros;

    if (!x)
        return 32;

    numZeros = 0;
    while (!(x & 0x80000000))
    {
        numZeros++;
        x <<= 1;
    }

    return numZeros;
}

//#error Unsupported platform in assembly.h

#endif  /* platforms */

#endif /* _ASSEMBLY_H */
