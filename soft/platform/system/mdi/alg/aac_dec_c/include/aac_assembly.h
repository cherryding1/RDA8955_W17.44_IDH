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

typedef long long Word64;

#endif

static __inline__ int MULSHIFT32(int x, int y)
{
    int z;

    z = (Word64)x * (Word64)y >> 32;

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
        return 32;

    /* count leading zeros with binary search (function should be 17 ARM instructions total) */
    numZeros = 1;
    if (!((unsigned int)x >> 16))   { numZeros += 16; x <<= 16; }
    if (!((unsigned int)x >> 24))   { numZeros +=  8; x <<=  8; }
    if (!((unsigned int)x >> 28))   { numZeros +=  4; x <<=  4; }
    if (!((unsigned int)x >> 30))   { numZeros +=  2; x <<=  2; }

    numZeros -= ((unsigned int)x >> 31);

    return numZeros;
}

typedef union _U64
{
    Word64 w64;
    struct
    {
        /* x86 = little endian */
        unsigned int lo32;
        signed int   hi32;
    } r;
} U64;

static __inline Word64 MADD64(Word64 sum64, int x, int y)
{
    sum64 += (Word64)x * (Word64)y;

    return sum64;
}

#elif defined(__GNUC__) && (defined(__powerpc__) || defined(__POWERPC__))

typedef long long Word64;

static __inline__ int MULSHIFT32(int x, int y)
{
    int z;

    z = (Word64)x * (Word64)y >> 32;

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
        return 32;

    /* count leading zeros with binary search (function should be 17 ARM instructions total) */
    numZeros = 1;
    if (!((unsigned int)x >> 16))   { numZeros += 16; x <<= 16; }
    if (!((unsigned int)x >> 24))   { numZeros +=  8; x <<=  8; }
    if (!((unsigned int)x >> 28))   { numZeros +=  4; x <<=  4; }
    if (!((unsigned int)x >> 30))   { numZeros +=  2; x <<=  2; }

    numZeros -= ((unsigned int)x >> 31);

    return numZeros;
}

typedef union _U64
{
    Word64 w64;
    struct
    {
        /* PowerPC = big endian */
        signed int   hi32;
        unsigned int lo32;
    } r;
} U64;

static __inline Word64 MADD64(Word64 sum64, int x, int y)
{
    sum64 += (Word64)x * (Word64)y;

    return sum64;
}



#endif /* _ASSEMBLY_H */
