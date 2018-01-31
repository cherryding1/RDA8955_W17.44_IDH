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
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"
#include "vpp_efr_inv_sqrt.tab" /* Table for inv_sqrt() */

INT32 Inv_sqrt (       /* (o) : output value   */
    INT32 L_x          /* (i) : input value    */
)
{
    INT16 exp, i, a, tmp;
    INT32 L_y;
    register INT32 Ly_hi=0;
    register UINT32 Ly_lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Inv_sqrt);

    if (L_x <= (INT32) 0)
    {
        VPP_EFR_PROFILE_FUNCTION_EXIT(Inv_sqrt);
        return ((INT32) 0x3fffffffL);
    }

    exp = norm_l (L_x);
    //L_x = L_shl (L_x, exp);     /* L_x is normalize */
    L_x = L_SHL(L_x, exp);
    //exp = sub (30, exp);
    exp = SUB (30, exp);

    if ((exp & 1) == 0)         /* If exponent even -> shift right */
    {
        // L_x = L_shr (L_x, 1);
        L_x = L_SHR_D(L_x, 1);
        // L_SHR(L_x, 1, L_x);
    }
    //exp = shr (exp, 1);
    exp = SHR_D(exp, 1);
    //exp = add (exp, 1);
    exp = ADD (exp, 1);

    //L_x = L_shr (L_x, 9);
    L_x = L_SHR_D(L_x, 9);
    //i = extract_h (L_x);        /* Extract b25-b31 */
    i = EXTRACT_H(L_x);
    //L_x = l_shr(L_x, 1);
    L_x = L_SHR_D(L_x, 1);
    //a = extract_l (L_x);        /* Extract b10-b24 */
    a = EXTRACT_L(L_x);
    a = a & (INT16) 0x7fff;

    //i = sub (i, 16);
    i = SUB (i, 16);

    //L_y = L_deposit_h (table[i]);         /* table[i] << 16          */
    //L_y = L_DEPOSIT_H(table[i]);

    Ly_lo = SHL(table[i],15); // L_DEPOSIT_H = sHL 16 + SHR 1 = sHL 15
    //tmp = sub (table[i], table[i + 1]); /* table[i] - table[i+1])  */
    tmp = SUB (table[i], table[i + 1]);
    //L_y = L_msu (L_y, tmp, a);  /* L_y -=  tmp*a*2         */
    //L_y = L_MSU(L_y, tmp, a);
    VPP_MLA16(Ly_hi,Ly_lo, tmp, -a);
    //L_y = L_shr (L_y, exp);     /* denormalization */
    L_y = L_SHR_V(VPP_SCALE64_TO_16(Ly_hi,Ly_lo), exp);

    VPP_EFR_PROFILE_FUNCTION_EXIT(Inv_sqrt);
    return (L_y);
}
