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






#include "vpp_hr_mathhalf.h"
#include "vpp_hr_typedefs.h"
#include "vpp_hr_mathhalf_macro.h"

/****************************************************************************
 *
 *     FUNCTION NAME: isLwLimit
 *
 *     PURPOSE:
 *
 *        Check to see if the input INT32 is at the
 *        upper or lower limit of its range.  i.e.
 *        0x7fff ffff or -0x8000 0000
 *
 *        Ostensibly this is a check for an overflow.
 *        This does not truly mean an overflow occurred,
 *        it means the value reached is the
 *        maximum/minimum value representable.  It may
 *        have come about due to an overflow.
 *
 *     INPUTS:
 *
 *       L_In               A INT32 input variable
 *
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        1 if input == 0x7fff ffff or -0x8000 0000
 *                          0 otherwise
 *
 *     KEYWORDS: saturation, limit
 *
 ***************************************************************************/

short  isLwLimit(INT32 L_In)
{

    INT32 L_ls;
    short  siOut;

    if (L_In != 0)
    {
        //L_ls = L_shl(L_In, 1);
        L_ls = SHL(L_In, 1);
        //if (L_sub(L_In, L_ls) == 0)
        if ( L_In == L_ls )
            siOut = 1;
        else
            siOut = 0;
    }
    else
    {
        siOut = 0;
    }
    return (siOut);
}

/****************************************************************************
 *
 *     FUNCTION NAME: isSwLimit
 *
 *     PURPOSE:
 *
 *        Check to see if the input INT16 is at the
 *        upper or lower limit of its range.  i.e.
 *        0x7fff or -0x8000
 *
 *        Ostensibly this is a check for an overflow.
 *        This does not truly mean an overflow occurred,
 *        it means the value reached is the
 *        maximum/minimum value representable.  It may
 *        have come about due to an overflow.
 *
 *     INPUTS:
 *
 *       swIn               A INT16 input variable
 *
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        1 if input == 0x7fff or -0x8000
 *                          0 otherwise
 *
 *     KEYWORDS: saturation, limit
 *
 ***************************************************************************/

short  isSwLimit(INT16 swIn)
{

    INT16 swls;
    short  siOut;

    if (swIn != 0)
    {
        //swls = shl(swIn, 1);
        swls = SHL(swIn, 1);
        if ( swIn == swls)          /* logical compare outputs 1/0 */
            siOut = 1;
        else
            siOut = 0;
    }
    else
    {
        siOut = 0;
    }
    return (siOut);

}

/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ll
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var1) and a 32 bit number
 *                 (L_var2), and return a 32 bit result.
 *
 *     INPUTS:
 *
 *       L_var1             A INT32 input variable
 *
 *       L_var2             A INT32 input variable
 *
 *     OUTPUTS:             none
 *
 *     IMPLEMENTATION:
 *
 *        Performs a 31x31 bit multiply, Complexity=24 Ops.
 *
 *        Let x1x0, or y1y0, be the two constituent halves
 *        of a 32 bit number.  This function performs the
 *        following:
 *
 *        low = ((x0 >> 1)*(y0 >> 1)) >> 16     (low * low)
 *        mid1 = [(x1 * (y0 >> 1)) >> 1 ]       (high * low)
 *        mid2 = [(y1 * (x0 >> 1)) >> 1]        (high * low)
 *        mid =  (mid1 + low + mid2) >> 14      (sum so far)
 *        output = (y1*x1) + mid                (high * high)
 *
 *
 *     RETURN VALUE:        A INT32 value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/

INT32 L_mpy_ll(INT32 L_var1, INT32 L_var2)
{
    INT16 swLow1,
          swLow2,
          swHigh1,
          swHigh2;
    INT32 L_varOut,
          L_low,
          L_mid1,
          L_mid2,
          L_mid;

    /*register INT32  s_hi=0;
    register UINT32 s_lo=0;
    INT32 L_varOuto,essai;//*/
    INT32 L_m;

    swLow1 = SHR(EXTRACT_L(L_var1), 1);
    swLow1 = SW_MAX & swLow1;

    swLow2 = SHR(EXTRACT_L(L_var2), 1);
    swLow2 = SW_MAX & swLow2;
    swHigh1 = EXTRACT_H(L_var1);
    swHigh2 = EXTRACT_H(L_var2);

    L_low = L_MULT(swLow1, swLow2);
    //L_low = L_shr(L_low, 16);
    L_low = SHR(L_low, 16);

    L_mid1 = L_MULT(swHigh1, swLow2);
    //L_mid1 = L_shr(L_mid1, 1);
    L_mid1 = SHR(L_mid1, 1);
    L_mid = L_ADD(L_mid1, L_low);

    L_mid2 = L_MULT(swHigh2, swLow1);
    //L_mid2 = L_shr(L_mid2, 1);
    L_mid2 = SHR(L_mid2, 1);
    L_mid = L_ADD(L_mid, L_mid2);

    //L_mid = L_shr(L_mid, 14);
    //L_mid = SHR(L_mid, 14);
    L_mid >>=  14 ;
    /*s_lo=SHR(L_mid ,1);
    L_varOut = L_mac(L_mid, swHigh1, swHigh2);
    VPP_MLA16(s_hi,s_lo, swHigh1, swHigh2);*/
    L_m= L_MULT(swHigh1, swHigh2);
    L_varOut = L_ADD(L_mid, L_m);//L_MAC(L_mid, swHigh1, swHigh2);//L_MLA_CARRY_SAT(s_hi,s_lo,(L_mid&1));//L_MLA_SAT(s_hi,s_lo);//


    /*if (L_varOuto!=L_varOut)
      essai=0;*/

    return (L_varOut);
}

/****************************************************************************
 *
 *     FUNCTION NAME: L_mpy_ls
 *
 *     PURPOSE:    Multiply a 32 bit number (L_var2) and a 16 bit
 *                 number (var1) returning a 32 bit result. L_var2
 *                 is truncated to 31 bits prior to executing the
 *                 multiply.
 *
 *     INPUTS:
 *
 *       L_var2             A INT32 input variable
 *
 *       var1               A INT16 input variable
 *
 *     OUTPUTS:             none
 *
 *     RETURN VALUE:        A INT32 value
 *
 *     KEYWORDS: mult,mpy,multiplication
 *
 ***************************************************************************/

INT32 L_mpy_ls(INT32 L_var2, INT16 var1)
{
    INT32 L_varOut;
    INT16 swtemp;

    /*register INT32  s_hi=0;
    register UINT32 s_lo=0;
    INT32 L_varOuto,L_varOut1;*/
    INT32 L_m;//

    //swtemp = shr(extract_l(L_var2), 1);
    swtemp = SHR(EXTRACT_L(L_var2), 1);
    swtemp = (short) 32767 & (short) swtemp;

    L_varOut = L_MULT(var1, swtemp);
    //L_varOut = L_shr(L_varOut, 15);
    //L_varOut = SHR(L_varOut, 15);
    L_varOut >>= 15;
    //L_varOut = L_mac(L_varOut, var1, EXTRACT_H(L_var2));
    L_m= L_MULT(var1, EXTRACT_H(L_var2));
    L_varOut = L_ADD(L_varOut, L_m);/**/
    //L_varOut = L_MAC(L_varOut, var1, EXTRACT_H(L_var2));


    /*s_lo=SHR(L_varOut ,1);//L_varOut;//
    VPP_MLA16(s_hi,s_lo, var1, EXTRACT_H(L_var2));
    L_varOut1 = L_MLA_SAT(s_hi,s_lo);//L_MLA_CARRY_SAT(s_hi, s_lo, L_varOut&1);//

    if (L_varOut&1)
    {
        L_varOut1++;
    }*/


    return (L_varOut);
}
