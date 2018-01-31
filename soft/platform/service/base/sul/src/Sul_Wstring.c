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















#include <sul.h>
#include <rfm.h>
#include<stdio.h>


#define _UPPER 0x0001
#define _LOWER 0x0002
#define _DIGIT 0x0004

#define     _SPACE          0x0008
#define     _PUNCT          0x0010
#define     _CONTROL     0x0020
#define     _BLANK          0x0040
#define     _HEX          0x0080
#if 0
#define HUGE_VAL    1.7976931348623157e+308
#define DBL_MIN         2.2250738585072014e-308
#define DBL_MAX         1.7976931348623158e+308
#else
#define HUGE_VAL    10000000
#define DBL_MIN         0
#define DBL_MAX         10000000
#endif
//#define L _T_MMI
#define Aa_Difference (_T_MMI'A'-_T_MMI'a')

#define towupper(c) ((( c >= _T_MMI'a' ) && ( c <= _T_MMI'z' )) ? c + Aa_Difference : c )
#define towlower(c) ((( c >= _T_MMI'A' ) && ( c<= _T_MMI'Z' )) ? c - Aa_Difference : c )

#define iswlower(c)  (( c ) >= _T_MMI'a' && ( c ) <= _T_MMI'z' )
#define iswupper(c)  (( c ) >= _T_MMI'A' && ( c ) <= _T_MMI'Z' )

#define iswdigit(c)      (( c ) >= _T_MMI'0' && ( c ) <= _T_MMI'9')
#define iswxdigit(c)         ((( c ) >= _T_MMI'0' && ( c) <= _T_MMI'9') || (( c ) >= _T_MMI'A' && ( c ) <=_T_MMI'F' ) || (( c ) >=_T_MMI'a' && ( c ) <=_T_MMI'f' ))

struct f64
{
    UINT32 low_word;
    UINT32 high_word;
};

#define F64_SIGN_SHIFT  31
#define F64_SIGN_MASK   1

#define F64_EXP_SHIFT   20
#define F64_EXP_MASK    0x7ff
#define F64_EXP_BIAS    1023
#define F64_EXP_MAX     2047

#define F64_MANT_SHIFT  0
#define F64_MANT_MASK   0xfffff

#define F64_GET_SIGN(fp)        (((fp)->high_word >> F64_SIGN_SHIFT) & \
                                         F64_SIGN_MASK)
#define F64_GET_EXP(fp)         (((fp)->high_word >> F64_EXP_SHIFT) & \
                                         F64_EXP_MASK)
#define F64_SET_EXP(fp, val) ((fp)->high_word= ((fp)->high_word & \
                                 ~(F64_EXP_MASK << F64_EXP_SHIFT)) |     \
 (((val) & F64_EXP_MASK) << F64_EXP_SHIFT))

#define F64_GET_MANT_LOW(fp)            ((fp)->low_word)
#define F64_SET_MANT_LOW(fp, val)       ((fp)->low_word= (val))
#define F64_GET_MANT_HIGH(fp)   (((fp)->high_word >> F64_MANT_SHIFT) & \
                                         F64_MANT_MASK)
#define F64_SET_MANT_HIGH(fp, val)      ((fp)->high_word= ((fp)->high_word & \
                                 ~(F64_MANT_MASK << F64_MANT_SHIFT)) |   \
                                 (((val) & F64_MANT_MASK) << F64_MANT_SHIFT))


typedef int int4;
typedef union
{
    int4 i[2];
    double x;
} mynumber;

#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412
#define __FLOAT_WORD_ORDER __BIG_ENDIAN
# if __FLOAT_WORD_ORDER == __BIG_ENDIAN
#  define BIG_ENDI 1
#  undef LITTLE_ENDI
#  define HIGH_HALF 0
#  define  LOW_HALF 1
# else
#  if __FLOAT_WORD_ORDER == __LITTLE_ENDIAN
#   undef BIG_ENDI
#   define LITTLE_ENDI 1
#   define HIGH_HALF 1
#   define  LOW_HALF 0
#  endif
# endif

#define r0  0
#define r1  1
#define r2  2
#define r3  3
#define r4  4
#define r5  5
#define r6  6
#define r7  7
#define r8  8
#define r9  9
#define r10 10
#define r11 11
#define r12 12
#define r13 13
#define r14 14
#define r15 15
#define r16 16
#define r17 17
#define r18 18
#define r19 19
#define r20 20
#define r21 21
#define r22 22
#define r23 23
#define r24 24
#define r25 25
#define r26 26
#define r27 27
#define r28 28
#define r29 29
#define r30 30
#define r31 31

/* Floating-point registers.  */
#define fp0 0
#define fp1 1
#define fp2 2
#define fp3 3
#define fp4 4
#define fp5 5
#define fp6 6
#define fp7 7
#define fp8 8
#define fp9 9
#define fp10    10
#define fp11    11
#define fp12    12
#define fp13    13
#define fp14    14
#define fp15    15
#define fp16    16
#define fp17    17
#define fp18    18
#define fp19    19
#define fp20    20
#define fp21    21
#define fp22    22
#define fp23    23
#define fp24    24
#define fp25    25
#define fp26    26
#define fp27    27
#define fp28    28
#define fp29    29
#define fp30    30
#define fp31    31

/* Condition code registers.  */
#define cr0 0
#define cr1 1
#define cr2 2
#define cr3 3
#define cr4 4
#define cr5 5
#define cr6 6
#define cr7 7

/* Vector registers. */
#define v0  0
#define v1  1
#define v2  2
#define v3  3
#define v4  4
#define v5  5
#define v6  6
#define v7  7
#define v8  8
#define v9  9
#define v10 10
#define v11 11
#define v12 12
#define v13 13
#define v14 14
#define v15 15
#define v16 16
#define v17 17
#define v18 18
#define v19 19
#define v20 20
#define v21 21
#define v22 22
#define v23 23
#define v24 24
#define v25 25
#define v26 26
#define v27 27
#define v28 28
#define v29 29
#define v30 30
#define v31 31

#define VRSAVE  256

/////////////////#define ABS(x) (x >= 0 ? x : -x)

static const float hugex = (const float)1.0e+30, tiny = (const float)1.0e-30;

typedef union
{
    float value;
    UINT32 word;
} ieee_float_shape_type;

#define GET_FLOAT_WORD(i,d)                 \
do {                                \
  ieee_float_shape_type gf_u;                   \
  gf_u.value = (d);                     \
  (i) = gf_u.word;                      \
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)                 \
do {                                \
  ieee_float_shape_type sf_u;                   \
  sf_u.word = (i);                      \
  (d) = sf_u.value;                     \
} while (0)


// ifdef.cpp : Defines the entry point for the console application.
//

//////////////////////////////////#include "stdafx.h"
///////////////#include "SUL_Wstring.h"
#include "string.h"
UINT16 *SUL_Wcschr(const UINT16 *str, UINT16 ch)
{
    while (0x00 != (*str))
    {
        if ((*str) == ch)
        {
            return ((UINT16 *)str);
        }
        str++;
    }
    return (NULL);
}

UINT16 *SUL_Wcsstr(const UINT16 *s, const UINT16 *b)
{
    UINT16 *x;
    UINT16 *y;
    UINT16 *c;

    x = (UINT16 *)s;
    while (*x)
    {
        if (*x == *b)
        {
            y = x;
            c = (UINT16 *)b;
            while (*y && *c && *y == *c)
            {
                c++;
                y++;
            }
            if (!*c)
                return x;
        }
        x++;
    }
    return NULL;
}

UINT16 *SUL_Wcsrchr(const UINT16 *str, UINT16 ch)
{
    UINT16 *sp = (UINT16 *)0;

    while (*str != 0)
    {
        if (*str == ch)
            sp = (UINT16 *)str;
        str++;
    }

    if (ch == 0)
        sp = (UINT16 *)str;

    return sp;
}

INT32 SUL_Wcsnicmp(const UINT16 *cs, const UINT16 *ct, UINT32 count)
{
    UINT16 *save = (UINT16 *)cs;

    while (towlower(*cs) == towlower(*ct) && (UINT32)(cs - save) < count)
    {
        if (*cs == 0)
        {
            return 0;
        }

        cs++;
        ct++;
    }
    return towlower(*cs) - towlower(*ct);

}

UINT32 SUL_Wcsnlen(const UINT16 *s, UINT32 count)
{

    UINT32 len = 0;

    while (s[len] != 0 && len < count)
    {
        len++;
    }

    return len;
}

UINT16 *SUL_Wcscat(UINT16 *dest, const UINT16 *src)
{

    INT32 i, j;

    j = 0;

    for (i = 0; 0 != dest[i]; i++)
        ;
    while (0 != src[j])
    {
        dest[i + j] = src[j];
        j++;
    }
    dest[i + j] = 0;
    return dest;
}

INT32 SUL_Wcscmp(const UINT16 *cs, const UINT16 *ct)
{
    while (*cs == *ct)
    {
        if (*cs == 0)
            return 0;

        cs++;
        ct++;
    }
    return* cs - *ct;

}

UINT16 *SUL_Wcscpy(UINT16 *str1, const UINT16 *str2)
{
    UINT16 *save = str1;

    for (; (*str1 = *str2); ++str2, ++str1)
        ;

    return save;
}

INT32 SUL_Wcsicmp(const UINT16 *cs, const UINT16 *ct)
{
    while (towlower(*cs) == towlower(*ct))
    {
        if (*cs == 0)
            return 0;
        cs++;
        ct++;
    }

    return towlower(*cs) - towlower(*ct);

}

UINT16 *SUL_Wcsupr(UINT16 *x)
{
    UINT16 *y = x;

    while (*y)
    {
        *y = towupper(*y);
        y++;
    }

    return x;
}

UINT16 *SUL_Wcslwr(UINT16 *x)
{
    UINT16 *y = x;

    while (*y)
    {
        *y = towlower(*y);
        y++;
    }
    return x;
}

UINT16 *SUL_Wcsncat(UINT16 *dest, const UINT16 *src, UINT32 count)
{
    UINT32 i, j;

    if (count != 0)
    {
        for (j = 0; 0 != dest[j]; j++)
            ;

        for (i = 0; i < count; i++)
        {
            dest[j + i] = src[i];
            if (src[i] == 0)
                return (dest);

        }
        dest[j + i] = 0;
    }
    return (dest);
}

INT32 SUL_Wcsncmp(const UINT16 *cs, const UINT16 *ct, UINT32 count)
{
    while ((*cs) == (*ct) && count > 0)
    {
        if (*cs == 0)
            return 0;
        cs++;
        ct++;
        count--;
    }
    return (*cs) - (*ct);

}

UINT16 *SUL_Wcstok(UINT16 *s, const UINT16 *ct)
{
    const UINT16 *spanp;
    INT32 c, sc;
    UINT16 *tok;
    static UINT16 *last;

    if (s == NULL && (s = last) == NULL)
        return (NULL);

    /*
     * Skip (span) leading ctiters (s += strspn(s, ct), sort of).
     */
cont:
    c = *s;
    s++;
    for (spanp = ct; (sc = *spanp) != 0; spanp++)
    {
        if (c == sc)
            goto cont;
    }

    if (c == 0)
    {
        /* no non-ctiter characters */
        last = NULL;
        return (NULL);
    }
    tok = s - 2;

    /*
     * Scan token (scan for ctiters: s += strcspn(s, ct), sort of).
     * Note that ct must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {

        c = *s;
        s += 2;
        spanp = ct;
        do
        {
            if ((sc = *spanp) == c)
            {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                last    = s;
                return (tok);
            }
            spanp += 2;
        }
        while (sc != 0);

    }

    /* NOTREACHED */
}
UINT32 SUL_Wcstoul(const UINT16 *cp, UINT16 **endp, UINT32 base)
{
    UINT32 result = 0, value;

    if (!base)
    {
        base = 10;
        if (*cp == _T_MMI'0')
        {
            base = 8;
            cp++;
            if ((*cp == _T_MMI'x') && iswxdigit(cp[1]))
            {
                cp++;
                base = 16;
            }
        }
    }
    while (iswxdigit(*cp) && (value = iswdigit(*cp) ? *cp - _T_MMI'0' : (iswlower(*cp)
                                      ? towupper(*cp) : *cp) - _T_MMI'A' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }
    if (endp)
        *endp = (UINT16 *)cp;
    return result;
}

const UINT16 _wtype[] =
{
    0,  /* <EOF>, 0xFFFF */
    _CONTROL, /* CTRL+@, 0x00 */
    _CONTROL, /* CTRL+A, 0x01 */
    _CONTROL, /* CTRL+B, 0x02 */
    _CONTROL, /* CTRL+C, 0x03 */
    _CONTROL, /* CTRL+D, 0x04 */
    _CONTROL, /* CTRL+E, 0x05 */
    _CONTROL, /* CTRL+F, 0x06 */
    _CONTROL, /* CTRL+G, 0x07 */
    _CONTROL, /* CTRL+H, 0x08 */
    _CONTROL | _SPACE,  /* CTRL+I, 0x09 */
    _CONTROL | _SPACE,  /* CTRL+J, 0x0a */
    _CONTROL | _SPACE,  /* CTRL+K, 0x0b */
    _CONTROL | _SPACE,  /* CTRL+L, 0x0c */
    _CONTROL | _SPACE,  /* CTRL+M, 0x0d */
    _CONTROL, /* CTRL+N, 0x0e */
    _CONTROL, /* CTRL+O, 0x0f */
    _CONTROL, /* CTRL+P, 0x10 */
    _CONTROL, /* CTRL+Q, 0x11 */
    _CONTROL, /* CTRL+R, 0x12 */
    _CONTROL, /* CTRL+S, 0x13 */
    _CONTROL, /* CTRL+T, 0x14 */
    _CONTROL, /* CTRL+U, 0x15 */
    _CONTROL, /* CTRL+V, 0x16 */
    _CONTROL, /* CTRL+W, 0x17 */
    _CONTROL, /* CTRL+X, 0x18 */
    _CONTROL, /* CTRL+Y, 0x19 */
    _CONTROL, /* CTRL+Z, 0x1a */
    _CONTROL, /* CTRL+[, 0x1b */
    _CONTROL, /* CTRL+\, 0x1c */
    _CONTROL, /* CTRL+], 0x1d */
    _CONTROL, /* CTRL+^, 0x1e */
    _CONTROL, /* CTRL+_, 0x1f */
    _SPACE | _BLANK,  /* ` ', 0x20 */
    _PUNCT, /* `!', 0x21 */
    _PUNCT, /* 0x22 */
    _PUNCT, /* `#', 0x23 */
    _PUNCT, /* `$', 0x24 */
    _PUNCT, /* `%', 0x25 */
    _PUNCT, /* `&', 0x26 */
    _PUNCT, /* 0x27 */
    _PUNCT, /* `(', 0x28 */
    _PUNCT, /* `)', 0x29 */
    _PUNCT, /* `*', 0x2a */
    _PUNCT, /* `+', 0x2b */
    _PUNCT, /* `,', 0x2c */
    _PUNCT, /* `-', 0x2d */
    _PUNCT, /* `.', 0x2e */
    _PUNCT, /* `/', 0x2f */
    _DIGIT | _HEX,  /* `0', 0x30 */
    _DIGIT | _HEX,  /* `1', 0x31 */
    _DIGIT | _HEX,  /* `2', 0x32 */
    _DIGIT | _HEX,  /* `3', 0x33 */
    _DIGIT | _HEX,  /* `4', 0x34 */
    _DIGIT | _HEX,  /* `5', 0x35 */
    _DIGIT | _HEX,  /* `6', 0x36 */
    _DIGIT | _HEX,  /* `7', 0x37 */
    _DIGIT | _HEX,  /* `8', 0x38 */
    _DIGIT | _HEX,  /* `9', 0x39 */
    _PUNCT, /* `:', 0x3a */
    _PUNCT, /* `;', 0x3b */
    _PUNCT, /* `<', 0x3c */
    _PUNCT, /* `=', 0x3d */
    _PUNCT, /* `>', 0x3e */
    _PUNCT, /* `?', 0x3f */
    _PUNCT, /* `@', 0x40 */
    _UPPER | _HEX,  /* `A', 0x41 */
    _UPPER | _HEX,  /* `B', 0x42 */
    _UPPER | _HEX,  /* `C', 0x43 */
    _UPPER | _HEX,  /* `D', 0x44 */
    _UPPER | _HEX,  /* `E', 0x45 */
    _UPPER | _HEX,  /* `F', 0x46 */
    _UPPER, /* `G', 0x47 */
    _UPPER, /* `H', 0x48 */
    _UPPER, /* `I', 0x49 */
    _UPPER, /* `J', 0x4a */
    _UPPER, /* `K', 0x4b */
    _UPPER, /* `L', 0x4c */
    _UPPER, /* `M', 0x4d */
    _UPPER, /* `N', 0x4e */
    _UPPER, /* `O', 0x4f */
    _UPPER, /* `P', 0x50 */
    _UPPER, /* `Q', 0x51 */
    _UPPER, /* `R', 0x52 */
    _UPPER, /* `S', 0x53 */
    _UPPER, /* `T', 0x54 */
    _UPPER, /* `U', 0x55 */
    _UPPER, /* `V', 0x56 */
    _UPPER, /* `W', 0x57 */
    _UPPER, /* `X', 0x58 */
    _UPPER, /* `Y', 0x59 */
    _UPPER, /* `Z', 0x5a */
    _PUNCT, /* `[', 0x5b */
    _PUNCT, /* 0x5c */
    _PUNCT, /* `]', 0x5d */
    _PUNCT, /* `^', 0x5e */
    _PUNCT, /* `_', 0x5f */
    _PUNCT, /* 0x60 */
    _LOWER | _HEX,  /* `a', 0x61 */
    _LOWER | _HEX,  /* `b', 0x62 */
    _LOWER | _HEX,  /* `c', 0x63 */
    _LOWER | _HEX,  /* `d', 0x64 */
    _LOWER | _HEX,  /* `e', 0x65 */
    _LOWER | _HEX,  /* `f', 0x66 */
    _LOWER, /* `g', 0x67 */
    _LOWER, /* `h', 0x68 */
    _LOWER, /* `i', 0x69 */
    _LOWER, /* `j', 0x6a */
    _LOWER, /* `k', 0x6b */
    _LOWER, /* `l', 0x6c */
    _LOWER, /* `m', 0x6d */
    _LOWER, /* `n', 0x6e */
    _LOWER, /* `o', 0x6f */
    _LOWER, /* `p', 0x70 */
    _LOWER, /* `q', 0x71 */
    _LOWER, /* `r', 0x72 */
    _LOWER, /* `s', 0x73 */
    _LOWER, /* `t', 0x74 */
    _LOWER, /* `u', 0x75 */
    _LOWER, /* `v', 0x76 */
    _LOWER, /* `w', 0x77 */
    _LOWER, /* `x', 0x78 */
    _LOWER, /* `y', 0x79 */
    _LOWER, /* `z', 0x7a */
    _PUNCT, /* `{', 0x7b */
    _PUNCT, /* `|', 0x7c */
    _PUNCT, /* `}', 0x7d */
    _PUNCT, /* `~', 0x7e */
    _CONTROL, /* 0x7f */
    0,  /* 0x80 */
    0,  /* 0x81 */
    0,  /* 0x82 */
    0,  /* 0x83 */
    0,  /* 0x84 */
    0,  /* 0x85 */
    0,  /* 0x86 */
    0,  /* 0x87 */
    0,  /* 0x88 */
    0,  /* 0x89 */
    0,  /* 0x8a */
    0,  /* 0x8b */
    0,  /* 0x8c */
    0,  /* 0x8d */
    0,  /* 0x8e */
    0,  /* 0x8f */
    0,  /* 0x90 */
    0,  /* 0x91 */
    0,  /* 0x92 */
    0,  /* 0x93 */
    0,  /* 0x94 */
    0,  /* 0x95 */
    0,  /* 0x96 */
    0,  /* 0x97 */
    0,  /* 0x98 */
    0,  /* 0x99 */
    0,  /* 0x9a */
    0,  /* 0x9b */
    0,  /* 0x9c */
    0,  /* 0x9d */
    0,  /* 0x9e */
    0,  /* 0x9f */
    0,  /* 0xa0 */
    0,  /* 0xa1 */
    0,  /* 0xa2 */
    0,  /* 0xa3 */
    0,  /* 0xa4 */
    0,  /* 0xa5 */
    0,  /* 0xa6 */
    0,  /* 0xa7 */
    0,  /* 0xa8 */
    0,  /* 0xa9 */
    0,  /* 0xaa */
    0,  /* 0xab */
    0,  /* 0xac */
    0,  /* 0xad */
    0,  /* 0xae */
    0,  /* 0xaf */
    0,  /* 0xb0 */
    0,  /* 0xb1 */
    0,  /* 0xb2 */
    0,  /* 0xb3 */
    0,  /* 0xb4 */
    0,  /* 0xb5 */
    0,  /* 0xb6 */
    0,  /* 0xb7 */
    0,  /* 0xb8 */
    0,  /* 0xb9 */
    0,  /* 0xba */
    0,  /* 0xbb */
    0,  /* 0xbc */
    0,  /* 0xbd */
    0,  /* 0xbe */
    0,  /* 0xbf */
    0,  /* 0xc0 */
    0,  /* 0xc1 */
    0,  /* 0xc2 */
    0,  /* 0xc3 */
    0,  /* 0xc4 */
    0,  /* 0xc5 */
    0,  /* 0xc6 */
    0,  /* 0xc7 */
    0,  /* 0xc8 */
    0,  /* 0xc9 */
    0,  /* 0xca */
    0,  /* 0xcb */
    0,  /* 0xcc */
    0,  /* 0xcd */
    0,  /* 0xce */
    0,  /* 0xcf */
    0,  /* 0xd0 */
    0,  /* 0xd1 */
    0,  /* 0xd2 */
    0,  /* 0xd3 */
    0,  /* 0xd4 */
    0,  /* 0xd5 */
    0,  /* 0xd6 */
    0,  /* 0xd7 */
    0,  /* 0xd8 */
    0,  /* 0xd9 */
    0,  /* 0xda */
    0,  /* 0xdb */
    0,  /* 0xdc */
    0,  /* 0xdd */
    0,  /* 0xde */
    0,  /* 0xdf */
    0,  /* 0xe0 */
    0,  /* 0xe1 */
    0,  /* 0xe2 */
    0,  /* 0xe3 */
    0,  /* 0xe4 */
    0,  /* 0xe5 */
    0,  /* 0xe6 */
    0,  /* 0xe7 */
    0,  /* 0xe8 */
    0,  /* 0xe9 */
    0,  /* 0xea */
    0,  /* 0xeb */
    0,  /* 0xec */
    0,  /* 0xed */
    0,  /* 0xee */
    0,  /* 0xef */
    0,  /* 0xf0 */
    0,  /* 0xf1 */
    0,  /* 0xf2 */
    0,  /* 0xf3 */
    0,  /* 0xf4 */
    0,  /* 0xf5 */
    0,  /* 0xf6 */
    0,  /* 0xf7 */
    0,  /* 0xf8 */
    0,  /* 0xf9 */
    0,  /* 0xfa */
    0,  /* 0xfb */
    0,  /* 0xfc */
    0,  /* 0xfd */
    0,  /* 0xfe */
    0 /* 0xff */
};

INT32 iswctype(UINT16 wc, UINT16 wctypeFlags)
{
    UINT16 *_pwctype = (UINT16 *)(_wtype + 1);

    return (_pwctype[(UINT8)(wc & 0xFF)] & wctypeFlags);
}

INT32 SUL_Iswspace(UINT16 wc)
{
    return (iswctype(wc, 9));
}

UINT16 *SUL_Ltow(long value, UINT16 *string, INT32 radix)
{
    unsigned long val;
    INT32 negative;
    UINT16 buffer[33];
    UINT16 *pos;
    UINT16 digit;
    if (value < 0 && radix == 10)
    {
        negative = 1;
        val      = -value;
    }
    else
    {
        negative = 0;
        val      = value;
    } /* if */

    pos  = &buffer[32];
    *pos = '\0';

    do
    {
        digit = (UINT16)(val % radix);
        val   = val / radix;
        if (digit < 10)
        {
            *--pos = '0' + digit;
        }
        else
        {
            *--pos = 'a' + digit - 10;
        } /* if */
    }
    while (val != 0L);

    if (negative)
    {
        *--pos = '-';
    } /* if */

    if (string != NULL)
    {
        memcpy(string, pos, (&buffer[32] - pos + 1) * sizeof(UINT16));
    }

    return string;
}

UINT16 *SUL_Itow(INT32 value, UINT16 *string, INT32 radix)
{
    return SUL_Ltow(value, string, radix);
}

double SUL_Modf(double value, double *iptr)
{
    struct f64 *f64p;
    double tmp;
    INT32 exp;
    INT32 mask_bits;
    UINT32 mant;

    f64p = (struct f64 *)(UINT32)&value;

    exp = F64_GET_EXP(f64p);
    exp -= F64_EXP_BIAS;
    if (exp < 0)
    {
        *iptr = 0;
        return value;
    }
    mask_bits = 52 - exp;
    if (mask_bits <= 0)
    {
        *iptr = value;
        return 0;
    }
    tmp = value;
    if (mask_bits >= 32)
    {
        F64_SET_MANT_LOW(f64p, 0);
        mask_bits -= 32;
        mant = F64_GET_MANT_HIGH(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_HIGH(f64p, mant);
    }
    else
    {
        mant = F64_GET_MANT_LOW(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_LOW(f64p, mant);
    }
    *iptr = value;

    return (tmp - value) < (double)0 ? value - (double)1.0 : value;


}

double SUL_Floor(double x)
{
    double val;
    double value = x;

    double *iptr = &val;
    struct f64 *f64p;
    double tmp;
    INT32 exp;
    INT32 mask_bits;
    UINT32 mant;

    f64p = (struct f64 *)(UINT32)&value;


    exp = F64_GET_EXP(f64p);
    exp -= F64_EXP_BIAS;
    if (exp < 0)
    {
        *iptr = 0;
        return value < 0 ? -1.0 : 0 ;
    }
    mask_bits = 52 - exp;
    if (mask_bits <= 0)
    {
        *iptr = value;

        //return 0;
        return value;
    }
    tmp = value;
    if (mask_bits >= 32)
    {
        F64_SET_MANT_LOW(f64p, 0);
        mask_bits -= 32;
        mant = F64_GET_MANT_HIGH(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_HIGH(f64p, mant);
    }
    else
    {
        mant = F64_GET_MANT_LOW(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_LOW(f64p, mant);
    }
    *iptr = value;

    return (value > tmp ) ? (double)(*iptr - 1.0) : *iptr;
}


UINT32 SUL_FloorEX(double z)
{
    double result = SUL_Floor(z);


    UINT32 y = 0;
    struct f64 *f64p = NULL;

    f64p = (struct f64 *)(UINT32)&result;
    f64p->high_word = f64p->high_word & 0x00000000;
    y = (UINT32)(result);
    if((z > 0) && (z < y))
    {
        y -= 1;
        return y;
    }

    return y;

}



double SUL_Ceil(double x)
{
    double val = 0;
    double value = x;

    double *iptr = &val;
    struct f64 *f64p = NULL;
    double tmp = 0;
    INT32 exp = 0;
    INT32 mask_bits = 0;
    UINT32 mant = 0;

    f64p = (struct f64 *)(UINT32)&value;

    exp = F64_GET_EXP(f64p);
    exp -= F64_EXP_BIAS;
    if (exp < 0)
    {
        *iptr = 0;

        return value > 0 ? +1.0 : 0 ;
    }
    mask_bits = 52 - exp;
    if (mask_bits <= 0)
    {
        *iptr = value;

        return value;
    }
    tmp = value;
    if (mask_bits >= 32)
    {
        F64_SET_MANT_LOW(f64p, 0);
        mask_bits -= 32;
        mant = F64_GET_MANT_HIGH(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_HIGH(f64p, mant);
    }
    else
    {
        mant = F64_GET_MANT_LOW(f64p);
        mant &= ~((1 << mask_bits) - 1);
        F64_SET_MANT_LOW(f64p, mant);
    }
    *iptr = value;


    return (tmp  > value) ? *iptr + 1.0 : *iptr;
}


UINT32 SUL_CeilEX(double z)
{
    VOLATILE double result = SUL_Ceil(z);
    UINT32 y = 0;
    //struct f64* f64p = NULL;

    //f64p = (struct f64*)(UINT32)&result;
    //f64p->high_word = f64p->high_word&0x00000000;
    y = (UINT32)(result);
    if((z > 0) && (y < z))
    {
        y += 1;
        return y;
    }

    if (!(y & 0x80000000))//result>0
    {
        return y;
    }
    return y;

}


/* Get two 32 bit ints from a double.  */
#if __FLOAT_WORD_ORDER == __BIG_ENDIAN

typedef union
{
    double value;
    struct
    {
        UINT32 msw;
        UINT32 lsw;
    } parts;
} ieee_double_shape_type;

#endif

#if __FLOAT_WORD_ORDER == __PDP_ENDIAN

typedef union
{
    double value;
    struct
    {
        UINT32 lsw;
        UINT32 msw;
    } parts;
} ieee_double_shape_type;

#endif

#define EXTRACT_WORDS(ix0,ix1,d)                \
do {                                \
  ieee_double_shape_type ew_u;                  \
  ew_u.value = (d);                     \
  (ix0) = ew_u.parts.msw;                   \
  (ix1) = ew_u.parts.lsw;                   \
} while (0)

// Debug
float fabsf(float a)
{
    if (a >= 0)
    {
        return a;
    }
    else
    {
        return 0 - a;
    }
}
int
__isinf(double x)
{
    INT32 hx, lx;

    EXTRACT_WORDS(hx, lx, x);
    lx |= (hx & 0x7fffffff) ^ 0x7ff00000;
    lx |= -lx;
    return~ (lx >> 31) & (hx >> 30);
}

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)                 \
do {                                \
  ieee_float_shape_type gf_u;                   \
  gf_u.value = (d);                     \
  (i) = gf_u.word;                      \
} while (0)

int
__isinff(float x)
{
    INT32 ix, t;

    GET_FLOAT_WORD(ix, x);
    t = ix & 0x7fffffff;
    t ^= 0x7f800000;
    t |= -t;
    return~ (t >> 31) & (ix >> 30);
}

#define isinf(x) \
     (sizeof (x) == sizeof (float) ? __isinff (x) : __isinf (x))

static const float
bp[] = { 1.0, 1.5, }, dp_h[] = {0.0, 5.84960938e-01,},  /* 0x3f15c000 */

                               dp_l[] =
{
    0.0, 1.56322085e-06,
},  /* 0x35d1cfdc */

zero = 0.0, one = 1.0, two = 2.0, two24 = 16777216.0, /* 0x4b800000 */
/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
L1      = 6.0000002384e-01,  /* 0x3f19999a */
L2      = 4.2857143283e-01,  /* 0x3edb6db7 */
L3      = 3.3333334327e-01,  /* 0x3eaaaaab */
L4      = 2.7272811532e-01,  /* 0x3e8ba305 */
L5      = 2.3066075146e-01,  /* 0x3e6c3255 */
L6      = 2.0697501302e-01,  /* 0x3e53f142 */
P1      = 1.6666667163e-01,  /* 0x3e2aaaab */
P2      = -2.7777778450e-03, /* 0xbb360b61 */
P3      = 6.6137559770e-05,  /* 0x388ab355 */
P4      = -1.6533901999e-06, /* 0xb5ddea0e */
P5      = 4.1381369442e-08,  /* 0x3331bb4c */
lg2     = 6.9314718246e-01, /* 0x3f317218 */
lg2_h   = 6.93145752e-01, /* 0x3f317200 */
lg2_l   = 1.42860654e-06, /* 0x35bfbe8c */
ovt     = 4.2995665694e-08, /* -(128-log2(ovfl+.5ulp)) */
cp      = 9.6179670095e-01,  /* 0x3f76384f =2/(3ln2) */
cp_h    = 9.6179199219e-01,  /* 0x3f763800 =head of cp */
cp_l    = 4.7017383622e-06,  /* 0x369dc3a0 =tail of cp_h */
ivln2   = 1.4426950216e+00, /* 0x3fb8aa3b =1/ln2 */
ivln2_h = 1.4426879883e+00, /* 0x3fb8aa00 =16b 1/ln2 */
ivln2_l = 7.0526075433e-06; /* 0x36eca570 =1/ln2 tail */

float __ieee754_sqrtf(float x)
{
    float z;
    INT32 sign = (int)0x80000000;
    INT32 ix, s, q, m, t, i;
    UINT32 r;

    GET_FLOAT_WORD(ix, x);

    /* take care of Inf and NaN */
    if ((ix & 0x7f800000) == 0x7f800000)
    {
        return x * x + x;  /* sqrt(NaN)=NaN, sqrt(+inf)=+inf
                         sqrt(-inf)=sNaN */
    }

    /* take care of zero */
    if (ix <= 0)
    {
        if ((ix & (~sign)) == 0)
            return x; /* sqrt(+-0) = +-0 */
        else if (ix < 0)
            return (x - x) / (x - x); /* sqrt(-ve) = sNaN */
    }

    /* normalize x */
    m = (ix >> 23);
    if (m == 0)
    {
        /* subnormal x */
        for (i = 0; (ix & 0x00800000) == 0; i++)
            ix <<= 1;
        m -= i - 1;
    }
    m -= 127; /* unbias exponent */
    ix = (ix & 0x007fffff) | 0x00800000;
    if (m & 1)  /* odd m, double x to make it even */
        ix += ix;
    m >>= 1;  /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix += ix;
    q = s = 0;  /* q = sqrt(x) */
    r = 0x01000000; /* r = moving bit from right to left */

    while (r != 0)
    {
        t = s + r;
        if (t <= ix)
        {
            s = t + r;
            ix -= t;
            q += r;
        }
        ix += ix;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if (ix != 0)
    {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one)
        {
            z = one + tiny;
            if (z > one)
                q += 2;
            else
                q += (q & 1);
        }
    }
    ix = (q >> 1) + 0x3f000000;
    ix += (m << 23);
    SET_FLOAT_WORD(z, ix);
    return z;
}
static const float

two25  = ( const float ) 3.355443200e+07,  /* 0x4c000000 */
twom25 = ( const float )2.9802322388e-08;  /* 0x33000000 */
float
__copysignf(float x, float y)
{
    // __asm ("cpys %1, %2, %0" : "=f" (x) : "f" (y), "f" (x));
    return x;
}

float __scalbnf(float x, int n)
{
    INT32 k, ix;

    GET_FLOAT_WORD(ix, x);
    k = (ix & 0x7f800000) >> 23;  /* extract exponent */
    if (k == 0)
    {
        /* 0 or subnormal x */
        if ((ix & 0x7fffffff) == 0)
            return x; /* +-0 */
        x *= two25;
        GET_FLOAT_WORD(ix, x);
        k = ((ix & 0x7f800000) >> 23) - 25;
    }
    if (k == 0xff)
        return x + x; /* NaN or Inf */
    k = k + n;
    if (n > 50000 || k > 0xfe)
        return hugex * __copysignf(hugex, x); /* overflow  */
    if (n < -50000)
        return tiny * __copysignf(tiny, x); /* underflow */
    if (k > 0)  /* normal result */
    {
        SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
        return x;
    }
    if (k <= -25)
        return tiny * __copysignf(tiny, x); /* underflow */
    k += 25;  /* subnormal result */
    SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
    return x * twom25;
}

float __ieee754_powf(float x, float y)
{
    float z, ax, z_h, z_l, p_h, p_l;
    float y1, t1, t2, r, s, t, u, v, w;
    INT32 i, j, k, yisint, n;
    INT32 hx, hy, ix, iy, is;

    GET_FLOAT_WORD(hx, x);
    GET_FLOAT_WORD(hy, y);
    ix = hx & 0x7fffffff;
    iy = hy & 0x7fffffff;
    //CSW_TRACE(_CSW|TLEVEL(MMI_TS_ID)|TDB|TNB_ARG(0), TSTR("__ieee754_powf----begin",0x080000de));
    /* y==zero: x**0 = 1 */
    if (iy == 0)
        return one;

    /* x==+-1 */
    if (x == 1.0)
        return one;
    if (x == -1.0 && isinf(y))
        return one;

    /* +-NaN return x+y */
    if (ix > 0x7f800000 || iy > 0x7f800000)
        return x + y;

    /* determine if y is an odd int when x < 0
     * yisint = 0 ... y is not an integer
     * yisint = 1 ... y is an odd int
     * yisint = 2 ... y is an even int
     */
    yisint = 0;
    if (hx < 0)
    {
        if (iy >= 0x4b800000)
            yisint = 2; /* even integer y */
        else if (iy >= 0x3f800000)
        {
            k = (iy >> 23) - 0x7f;  /* exponent */
            j = iy >> (23 - k);
            if ((j << (23 - k)) == iy)
                yisint = 2 - (j & 1);
        }
    }

    /* special value of y */
    if (iy == 0x7f800000)
    {
        /* y is +-inf */
        if (ix == 0x3f800000)
            return y - y; /* inf**+-1 is NaN */
        else if (ix > 0x3f800000) /* (|x|>1)**+-inf = inf,0 */
            return (hy >= 0) ? y : zero;
        else  /* (|x|<1)**-,+inf = inf,0 */
            return (hy < 0) ? -y : zero;
    }
    if (iy == 0x3f800000)
    {
        /* y is  +-1 */
        if (hy < 0)
            return one / x;
        else
            return x;
    }
    if (hy == 0x40000000)
        return x * x; /* y is  2 */
    if (hy == 0x3f000000)
    {
        /* y is  0.5 */
        if (hx >= 0)  /* x >= +0 */
            return __ieee754_sqrtf(x);
    }

    ax = fabsf(x);

    /* special value of x */
    if (ix == 0x7f800000 || ix == 0 || ix == 0x3f800000)
    {
        z = ax; /* x is +-0,+-inf,+-1 */
        if (hy < 0)
            z = one / z;  /* z = (1/|x|) */
        if (hx < 0)
        {
            if (((ix - 0x3f800000) | yisint) == 0)
            {
                z = (z - z) / (z - z);  /* (-1)**non-int is NaN */
            }
            else if (yisint == 1)
                z = -z; /* (x<0)**odd = -(|x|**odd) */
        }
        return z;
    }

    /* (x<0)**(non-int) is NaN */
    if (((((UINT32)hx >> 31) - 1) | yisint) == 0)
        return (x - x) / (x - x);

    /* |y| is huge */
    if (iy > 0x4d000000)
    {
        /* if |y| > 2**27 */
        /* over/underflow if x is not close to one */
        if (ix < 0x3f7ffff8)
            return (hy < 0) ? hugex * hugex : tiny * tiny;
        if (ix > 0x3f800007)
            return (hy > 0) ? hugex * hugex : tiny * tiny;

        /* now |1-x| is tiny <= 2**-20, suffice to compute
           log(x) by x-x^2/2+x^3/3-x^4/4 */
        t  = x - 1;  /* t has 20 trailing zeros */
        w  = (t * t) * ((float)0.5 - t * ((float)0.333333333333 - t * (float)0.25));
        u  = ivln2_h * t;  /* ivln2_h has 16 sig. bits */
        v  = t * ivln2_l - w * ivln2;
        t1 = u + v;
        GET_FLOAT_WORD(is, t1);
        SET_FLOAT_WORD(t1, is & 0xfffff000);
        t2 = v - (t1 - u);
    }
    else
    {
        float s2, s_h, s_l, t_h, t_l;

        n = 0;

        /* take care subnormal number */
        if (ix < 0x00800000)
        {
            ax *= two24;
            n -= 24;
            GET_FLOAT_WORD(ix, ax);
        }
        n += ((ix) >> 23) - 0x7f;
        j = ix & 0x007fffff;

        /* determine interval */
        ix = j | 0x3f800000;  /* normalize ix */
        if (j <= 0x1cc471)
            k = 0;  /* |x|<sqrt(3/2) */
        else if (j < 0x5db3d7)
            k = 1;  /* |x|<sqrt(3)   */
        else
        {
            k = 0;
            n += 1;
            ix -= 0x00800000;
        }
        SET_FLOAT_WORD(ax, ix);

        /* compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
        u   = ax - bp[k]; /* bp[0]=1.0, bp[1]=1.5 */
        v   = one / (ax + bp[k]);
        s   = u * v;
        s_h = s;
        GET_FLOAT_WORD(is, s_h);
        SET_FLOAT_WORD(s_h, is & 0xfffff000);

        /* t_h=ax+bp[k] High */
        SET_FLOAT_WORD(t_h, ((ix >> 1) | 0x20000000) + 0x0040000 + (k << 21));
        t_l = ax - (t_h - bp[k]);
        s_l = v * ((u - s_h * t_h) - s_h * t_l);

        /* compute log(ax) */
        s2 = s * s;
        r  = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6)))));
        r += s_l * (s_h + s);
        s2  = s_h * s_h;
        t_h = (float)3.0 + s2 + r;
        GET_FLOAT_WORD(is, t_h);
        SET_FLOAT_WORD(t_h, is & 0xfffff000);
        t_l = r - ((t_h - (float)3.0) - s2);

        /* u+v = s*(1+...) */
        u = s_h * t_h;
        v = s_l * t_h + t_l * s;

        /* 2/(3log2)*(s+...) */
        p_h = u + v;
        GET_FLOAT_WORD(is, p_h);
        SET_FLOAT_WORD(p_h, is & 0xfffff000);
        p_l = v - (p_h - u);
        z_h = cp_h * p_h; /* cp_h+cp_l = 2/(3*log2) */
        z_l = cp_l * p_h + p_l * cp + dp_l[k];

        /* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
        t  = (float)n;
        t1 = (((z_h + z_l) + dp_h[k]) + t);
        GET_FLOAT_WORD(is, t1);
        SET_FLOAT_WORD(t1, is & 0xfffff000);
        t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);
    }

    s = one;  /* s (sign of result -ve**odd) = -1 else = 1 */
    if (((((UINT32)hx >> 31) - 1) | (yisint - 1)) == 0)
        s = -one; /* (-ve)**(odd int) */

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
    GET_FLOAT_WORD(is, y);
    SET_FLOAT_WORD(y1, is & 0xfffff000);
    p_l = (y - y1) * t1 + y * t2;
    p_h = y1 * t1;
    z   = p_l + p_h;
    GET_FLOAT_WORD(j, z);
    if (j > 0x43000000) /* if z > 128 */
        return s * hugex * hugex; /* overflow */
    else if (j == 0x43000000)
    {
        /* if z == 128 */
        if (p_l + ovt > z - p_h)
            return s * hugex * hugex; /* overflow */
    }
    else if ((j & 0x7fffffff) > 0x43160000) /* z <= -150 */
        return s * tiny * tiny; /* underflow */
    else if ((UINT32)j == 0xc3160000)
    {
        /* z == -150 */
        if (p_l <= z - p_h)
            return s * tiny * tiny; /* underflow */
    }

    /*
     * compute 2**(p_h+p_l)
     */
    i = j & 0x7fffffff;
    k = (i >> 23) - 0x7f;
    n = 0;
    if (i > 0x3f000000)
    {
        /* if |z| > 0.5, set n = [z+0.5] */
        n = j + (0x00800000 >> (k + 1));
        k = ((n & 0x7fffffff) >> 23) - 0x7f;  /* new k for n */
        SET_FLOAT_WORD(t, n & ~(0x007fffff >> k));
        n = ((n & 0x007fffff) | 0x00800000) >> (23 - k);
        if (j < 0)
            n = -n;
        p_h -= t;
    }
    t = p_l + p_h;
    GET_FLOAT_WORD(is, t);
    SET_FLOAT_WORD(t, is & 0xfffff000);
    u  = t * lg2_h;
    v  = (p_l - (t - p_h)) * lg2 + t * lg2_l;
    z  = u + v;
    w  = v - (z - u);
    t  = z * z;
    t1 = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    r  = (z * t1) / (t1 - two) - (w + z * w);
    z  = one - (r - z);
    GET_FLOAT_WORD(j, z);
    j += (n << 23);
    if ((j >> 23) <= 0)
        z = __scalbnf(z, n);  /* subnormal output */
    else
        SET_FLOAT_WORD(z, j);

    //CSW_TRACE(_CSW|TLEVEL(MMI_TS_ID)|TDB|TNB_ARG(0), TSTR("__ieee754_powf----endif",0x080000df));
    return s * z;
}

float SUL_Powf(float x, float y)
{
    return __ieee754_powf( x, y );
}


