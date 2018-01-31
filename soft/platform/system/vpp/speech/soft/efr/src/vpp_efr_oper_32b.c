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





#include "cs_types.h"
#include "vpp_efr_basic_op.h"
#include "vpp_efr_oper_32b.h"

#include "vpp_efr_basic_macro.h"

/*****************************************************************************
 *                                                                           *
 *  Function L_Extract()                                                     *
 *                                                                           *
 *  Extract from a 32 bit integer two 16 bit DPF.                            *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   L_32      : 32 bit integer.                                             *
 *               0x8000 0000 <= L_32 <= 0x7fff ffff.                         *
 *   hi        : b16 to b31 of L_32                                          *
 *   lo        : (L_32 - hi<<16)>>1                                          *
 *****************************************************************************
*/

void L_Extract (INT32 L_32, INT16 *hi, INT16 *lo)
{
    //*hi = extract_h (L_32);
    //*hi = EXTRACT_H(L_32);
    //*lo = extract_l (L_msu (L_shr (L_32, 1), *hi, 16384));
    //*lo = EXTRACT_L( L_MSU (L_SHR(L_32, 1), *hi, 0x4000));

    *hi = L_SHR_D(L_32, 16);
    *lo = (INT16)((L_32&0xffff)>>1) ;

    return;
}

/*****************************************************************************
 *                                                                           *
 *  Function L_Comp()                                                        *
 *                                                                           *
 *  Compose from two 16 bit DPF a 32 bit integer.                            *
 *                                                                           *
 *     L_32 = hi<<16 + lo<<1                                                 *
 *                                                                           *
 *  Arguments:                                                               *
 *                                                                           *
 *   hi        msb                                                           *
 *   lo        lsf (with sign)                                               *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *             32 bit long signed integer (INT32) whose value falls in the  *
 *             range : 0x8000 0000 <= L_32 <= 0x7fff fff0.                   *
 *                                                                           *
 *****************************************************************************
*/

INT32 L_Comp (INT16 hi, INT16 lo)
{
    INT32 L_32;

    //L_32 = L_deposit_h (hi);
    L_32 = L_ADD (L_DEPOSIT_H(hi), SHL(lo,1)) ;
    //return (L_mac (L_32, lo, 1));
    return (L_32);       /* = hi<<16 + lo<<1 */
}

/*****************************************************************************
 * Function Mpy_32()                                                         *
 *                                                                           *
 *   Multiply two 32 bit integers (DPF). The result is divided by 2**31      *
 *                                                                           *
 *   L_32 = (hi1*hi2)<<1 + ( (hi1*lo2)>>15 + (lo1*hi2)>>15 )<<1              *
 *                                                                           *
 *   This operation can also be viewed as the multiplication of two Q31      *
 *   number and the result is also in Q31.                                   *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi1         hi part of first number                                      *
 *  lo1         lo part of first number                                      *
 *  hi2         hi part of second number                                     *
 *  lo2         lo part of second number                                     *
 *                                                                           *
 *****************************************************************************
*/

INT32 Mpy_32 (INT16 hi1, INT16 lo1, INT16 hi2, INT16 lo2)
{
    INT32 tmp=0;
    register INT32 test_hi=0;
    register UINT32 test_lo=0;

    //VPP_MLX16(ret_hi, ret_lo, MULT (hi1, lo2), 1);
    //VPP_MLA16(ret_hi, ret_lo, MULT (lo1, hi2), 1);
    //VPP_MLA16(ret_hi, ret_lo, hi1,hi2);

    VPP_MLX16(test_hi, test_lo, hi1, lo2);
    tmp = L_SHR_D((INT32)test_lo,15);
    VPP_MLX16(test_hi, test_lo, lo1, hi2);
    test_lo = tmp + L_SHR_D((INT32)test_lo,15);
    VPP_MLA16(test_hi, test_lo, hi1,hi2);


    return (VPP_SCALE64_TO_16(test_hi, test_lo));
}

/*****************************************************************************
 * Function Mpy_32_16()                                                      *
 *                                                                           *
 *   Multiply a 16 bit integer by a 32 bit (DPF). The result is divided      *
 *   by 2**15                                                                *
 *                                                                           *
 *                                                                           *
 *   L_32 = (hi1*lo2)<<1 + ((lo1*lo2)>>15)<<1                                *
 *                                                                           *
 * Arguments:                                                                *
 *                                                                           *
 *  hi          hi part of 32 bit number.                                    *
 *  lo          lo part of 32 bit number.                                    *
 *  n           16 bit number.                                               *
 *                                                                           *
 *****************************************************************************
*/

INT32 Mpy_32_16 (INT16 hi, INT16 lo, INT16 n)
{
    //INT32 L_32;
    INT32 ret_hi=0;
    UINT32 ret_lo=0;

    //L_32 = L_MULT(hi, n);
    //VPP_MLX16(ret_hi, ret_lo, hi, n);
    //L_32 = L_MAC(L_32, MULT(lo, n), 1);
    //VPP_MLA16(ret_hi, ret_lo, MULT(lo, n), 1);

    VPP_MLX16(ret_hi, ret_lo, lo, n);
    ret_lo = L_SHR_D((INT32)ret_lo,15);
    VPP_MLA16(ret_hi, ret_lo, hi, n);

    //return(L_32);
    return (VPP_SCALE64_TO_16(ret_hi, ret_lo));
}

/*****************************************************************************
 *                                                                           *
 *   Function Name : Div_32                                                  *
 *                                                                           *
 *   Purpose :                                                               *
 *             Fractional integer division of two 32 bit numbers.            *
 *             L_num / L_denom.                                              *
 *             L_num and L_denom must be positive and L_num < L_denom.       *
 *             L_denom = denom_hi<<16 + denom_lo<<1                          *
 *             denom_hi is a normalize number.                               *
 *                                                                           *
 *   Inputs :                                                                *
 *                                                                           *
 *    L_num                                                                  *
 *             32 bit long signed integer (INT32) whose value falls in the  *
 *             range : 0x0000 0000 < L_num < L_denom                         *
 *                                                                           *
 *    L_denom = denom_hi<<16 + denom_lo<<1      (DPF)                        *
 *                                                                           *
 *       denom_hi                                                            *
 *             16 bit positive normalized integer whose value falls in the   *
 *             range : 0x4000 < hi < 0x7fff                                  *
 *       denom_lo                                                            *
 *             16 bit positive integer whose value falls in the              *
 *             range : 0 < lo < 0x7fff                                       *
 *                                                                           *
 *   Return Value :                                                          *
 *                                                                           *
 *    L_div                                                                  *
 *             32 bit long signed integer (INT32) whose value falls in the  *
 *             range : 0x0000 0000 <= L_div <= 0x7fff ffff.                  *
 *                                                                           *
 *  Algorithm:                                                               *
 *                                                                           *
 *  - find = 1/L_denom.                                                      *
 *      First approximation: approx = 1 / denom_hi                           *
 *      1/L_denom = approx * (2.0 - L_denom * approx )                       *
 *                                                                           *
 *  -  result = L_num * (1/L_denom)                                          *
 *****************************************************************************
*/

INT32 Div_32 (INT32 L_num, INT16 denom_hi, INT16 denom_lo)
{
    INT16 approx, hi, lo, n_hi, n_lo;
    INT32 L_32;

    /* First approximation: 1 / L_denom = 1/denom_hi */

    approx = div_s ((INT16) 0x3fff, denom_hi);

    /* 1/L_denom = approx * (2.0 - L_denom * approx) */

    L_32 = Mpy_32_16 (denom_hi, denom_lo, approx);

    //L_32 = L_sub ((INT32) 0x7fffffffL, L_32);
    L_32 = L_SUB((INT32) 0x7fffffffL, L_32);

    L_Extract (L_32, &hi, &lo);

    L_32 = Mpy_32_16 (hi, lo, approx);

    /* L_num * (1/L_denom) */

    L_Extract (L_32, &hi, &lo);
    L_Extract (L_num, &n_hi, &n_lo);
    L_32 = Mpy_32 (n_hi, n_lo, hi, lo);
    //L_32 = L_shl (L_32, 2);
    L_32 = L_SHL_SAT(L_32, 2);

    return (L_32);
}





















