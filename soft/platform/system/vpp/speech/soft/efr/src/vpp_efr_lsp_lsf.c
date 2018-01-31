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

#include "vpp_efr_lsp_lsf.tab"          /* Look-up table for transformations */

void Lsf_lsp (
    INT16 lsf[],       /* (i) : lsf[m] normalized (range: 0.0<=val<=0.5) */
    INT16 lsp[],       /* (o) : lsp[m] (range: -1<=val<1)                */
    INT16 m            /* (i) : LPC order                                */
)
{
    INT16 i, ind, offset;
    //INT32 L_tmp;
    register INT32 hi=0;
    register UINT32 lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Lsf_lsp);

    for (i = 0; i < m; i++)
    {
        //ind = shr (lsf[i], 8);
        ind = SHR_D(lsf[i], 8);       /* ind    = b8-b15 of lsf[i] */
        offset = lsf[i] & 0x00ff;    /* offset = b0-b7  of lsf[i] */

        /* lsp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 256 */

        //L_tmp = L_MULT (SUB (table[ind + 1], table[ind]), offset);
        //lsp[i] = ADD(table[ind], EXTRACT_L (L_SHR_D(L_tmp, 9)));
        VPP_MLX16 (hi,lo,SUB (table[ind + 1], table[ind]),offset);
        lsp[i] = ADD(table[ind], (INT16)(L_SHR_D(lo, 8)));


    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Lsf_lsp);
    return;
}

void Lsp_lsf (
    INT16 lsp[],       /* (i)  : lsp[m] (range: -1<=val<1)                */
    INT16 lsf[],       /* (o)  : lsf[m] normalized (range: 0.0<=val<=0.5) */
    INT16 m            /* (i)  : LPC order                                */
)
{
    INT16 i, diff, ind =63;   /* begin at end of table -1 */
    //INT32 L_tmp;
    register INT32 hi=0;
    register UINT32 lo=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Lsp_lsf);

    for (i = m - 1; i >= 0; i--)
    {
        /* find value in table that is just greater than lsp[i] */
        //while (SUB (table[ind], lsp[i]) < 0) {ind--;}
        diff = SUB (table[ind], lsp[i]);
        while(diff < 0)
        {
            diff = SUB (table[--ind], lsp[i]);
        }

        /*acos(lsp[i])= ind*256 +((lsp[i]-table[ind])*slope[ind] )/4096 */
        //L_tmp = L_MULT (SUB (lsp[i], table[ind]), slope[ind]);
        //lsf[i] = ROUND(L_SHL_SAT(L_tmp, 3));
        //lsf[i] = lsf[i] + SHL(ind, 8);

        VPP_MLX16 (hi,lo,-diff,slope[ind]);
        lsf[i] = (INT16)(L_SHR_D(L_ADD((INT32)lo,0x800),12));
        lsf[i] += SHL(ind, 8);

    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Lsp_lsf);
    return;
}
