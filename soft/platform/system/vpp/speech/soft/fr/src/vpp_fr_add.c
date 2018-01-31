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









#include        <stdio.h>
#include        <assert.h>

#include        "vpp_fr_private.h"
#include        "vpp_fr_proto.h"

#define saturate(x)     \
        ((x) < MIN_WORD ? MIN_WORD : (x) > MAX_WORD ? MAX_WORD: (x))



/*INT16 gsm_add P2((a,b), INT16 a, INT16 b)
{
        INT32 sum = a + b;
        return (INT16) saturate(sum);
}*/


/*INT16 gsm_sub P2((a,b), INT16 a, INT16 b)
{
        INT32 diff = a - b;
        return (INT16) saturate(diff);
}*/

INT16 gsm_mult P2((a,b), INT16 a, INT16 b)
{
    if (a == MIN_WORD && b == MIN_WORD) return MAX_WORD;
    else return (INT16) SASR( (INT32)a * (INT32)b, 15 );
}



/*INT16 gsm_mult_r P2((a,b), INT16 a, INT16 b)
{
        if (b == MIN_WORD && a == MIN_WORD) return MAX_WORD;
        else {
                INT32 prod = (INT32)a * (INT32)b + 16384;
                prod >>= 15;
                return (INT16) (prod & 0xFFFF);
        }
}*/

/*INT16 gsm_abs P1((a), INT16 a)
{
        return a < 0 ? (a == MIN_WORD ? MAX_WORD : -a) : a;
}*/

/*INT32 gsm_L_mult P2((a,b),INT16 a, INT16 b)
{
        //assert( a != MIN_WORD || b != MIN_WORD );
        return ((INT32)a * (INT32)b) << 1;
}*/

/*INT32 gsm_L_add P2((a,b), INT32 a, INT32 b)
{
        if (a < 0) {
                if (b >= 0) return a + b;
                else {
                        UINT32 A = (UINT32)-(a + 1) + (UINT32)-(b + 1);
                        return A >= MAX_LONGWORD ? MIN_LONGWORD :-(INT32)A-2;
                }
        }
        else if (b <= 0) return a + b;
        else {
                UINT32 A = (UINT32)a + (UINT32)b;
                return A > MAX_LONGWORD ? MAX_LONGWORD : A;
        }
}*/

INT32 gsm_L_sub P2((a,b), INT32 a, INT32 b)
{
    if (a >= 0)
    {
        if (b >= 0) return a - b;
        else
        {
            /* a>=0, b<0 */

            UINT32 A = (UINT32)a + -(b + 1);
            return A >= MAX_LONGWORD ? MAX_LONGWORD : (A + 1);
        }
    }
    else if (b <= 0) return a - b;
    else
    {
        /* a<0, b>0 */

        UINT32 A = (UINT32)-(a + 1) + b;
        return A >= MAX_LONGWORD ? MIN_LONGWORD : -((INT32) A) - 1;
    }
}

static unsigned char bitoff[ 256 ] =
{
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

INT16 gsm_norm P1((a), INT32 a )
/*
 * the number of left shifts needed to normalize the 32 bit
 * variable L_var1 for positive values on the interval
 *
 * with minimum of
 * minimum of 1073741824  (01000000000000000000000000000000) and
 * maximum of 2147483647  (01111111111111111111111111111111)
 *
 *
 * and for negative values on the interval with
 * minimum of -2147483648 (-10000000000000000000000000000000) and
 * maximum of -1073741824 ( -1000000000000000000000000000000).
 *
 * in order to normalize the result, the following
 * operation must be done: L_norm_var1 = L_var1 << norm( L_var1 );
 *
 * (That's 'ffs', only from the left, not the right..)
 */
{
    //assert(a != 0);

    if (a < 0)
    {
        if (a <= -1073741824) return 0;
        a = ~a;
    }

    return    a & 0xffff0000
              ? ( a & 0xff000000
                  ?  -1 + bitoff[ 0xFF & (a >> 24) ]
                  :   7 + bitoff[ 0xFF & (a >> 16) ] )
              : ( a & 0xff00
                  ?  15 + bitoff[ 0xFF & (a >> 8) ]
                  :  23 + bitoff[ 0xFF & a ] );
}

INT32 gsm_L_asr P2((a,n), INT32 a, int n)
{
    if (n >= 32) return -(a < 0);
    if (n <= -32) return 0;
    if (n < 0) return a << -n;

#       ifdef   SASR
    return a >> n;
#       else
    if (a >= 0) return a >> n;
    else return -(INT32)( -(UINT32)a >> n );
#       endif
}

INT16 gsm_asr P2((a,n), INT16 a, int n)
{
    if (n >= 16) return -(a < 0);
    if (n <= -16) return 0;
    if (n < 0) return a << -n;

#       ifdef   SASR
    return a >> n;
#       else
    if (a >= 0) return a >> n;
    else return -(INT16)( -(UINT16)a >> n );
#       endif
}

INT32 gsm_L_asl P2((a,n), INT32 a, int n)
{
    if (n >= 32) return 0;
    if (n <= -32) return -(a < 0);
    //  if (n < 0) return gsm_asr((INT16) a, -n);
    if (n < 0) return gsm_L_asr( a, -n); //fts change 040119
    return a << n;
}

INT16 gsm_asl P2((a,n), INT16 a, int n)
{
    if (n >= 16) return 0;
    if (n <= -16) return -(a < 0);
    if (n < 0) return gsm_asr(a, -n);
    return a << n;
}

/*
 *  (From p. 46, end of section 4.2.5)
 *
 *  NOTE: The following lines gives [sic] one correct implementation
 *        of the div(num, denum) arithmetic operation.  Compute div
 *        which is the integer division of num by denum: with denum
 *        >= num > 0
 */

INT16 gsm_div P2((num,denum), INT16 num, INT16 denum)
{
    INT32       L_num   = num;
    INT32       L_denum = denum;
    INT16           div     = 0;
    int             k       = 15;

    /* The parameter num sometimes becomes zero.
     * Although this is explicitly guarded against in 4.2.5,
     * we assume that the result should then be zero as well.
     */

    /* assert(num != 0); */

    //assert(num >= 0 && denum >= num);
    if (num == 0)
        return 0;

    while (k--)
    {
        div   <<= 1;
        L_num <<= 1;

        if (L_num >= L_denum)
        {
            L_num -= L_denum;
            div++;
        }
    }

    return div;
}
