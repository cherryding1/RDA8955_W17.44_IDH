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
#include "vpp_efr_sig_proc.h"
#include "vpp_efr_basic_macro.h"

#include "vpp_efr_profile_codes.h"
#include "vpp_efr_debug.h"

#define M   10                  /* LP order */
#define MP1 11                  /* M+1 */

void Int_lpc (
    INT16 lsp_old[],   /* input : LSP vector at the 4th subframe
                           of past frame    */
    INT16 lsp_mid[],   /* input : LSP vector at the 2nd subframe
                           of present frame */
    INT16 lsp_new[],   /* input : LSP vector at the 4th subframe of
                           present frame */
    INT16 Az[]         /* output: interpolated LP parameters in
                           all subframes */
)
{
    INT16 i;
    INT16 lsp[M];

    VPP_EFR_PROFILE_FUNCTION_ENTER(Int_lpc);

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    for (i = 0; i < M; i++)
    {
        //lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_old[i], 1));
        lsp[i] =  ADD ((SHR_D(lsp_old[i], 1)),(SHR_D(lsp_mid[i], 1)));



    }

    Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += MP1;

    Lsp_Az (lsp_mid, Az);       /* Subframe 2 */
    Az += MP1;

    for (i = 0; i < M; i++)
    {
        //lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_new[i], 1));
        lsp[i] = ADD((SHR_D(lsp_mid[i], 1)), (SHR_D(lsp_new[i], 1)));

    }

    Lsp_Az (lsp, Az);           /* Subframe 3 */
    Az += MP1;

    Lsp_Az (lsp_new, Az);       /* Subframe 4 */

    VPP_EFR_PROFILE_FUNCTION_EXIT(Int_lpc);
    return;
}

/*----------------------------------------------------------------------*
 * Function Int_lpc2()                                                  *
 * ~~~~~~~~~~~~~~~~~~                                                   *
 * Interpolation of the LPC parameters.                                 *
 * Same as the previous function but we do not recompute Az() for       *
 * subframe 2 and 4 because it is already available.                    *
 *----------------------------------------------------------------------*/

void Int_lpc2 (
    INT16 lsp_old[],  /* input : LSP vector at the 4th subframe
                                 of past frame    */
    INT16 lsp_mid[],  /* input : LSP vector at the 2nd subframe
                                 of present frame */
    INT16 lsp_new[],  /* input : LSP vector at the 4th subframe of
                                 present frame */
    INT16 Az[]        /* output: interpolated LP parameters
                                 in subframes 1 and 3 */
)
{
    INT16 i;
    INT16 lsp[M];

    /*  lsp[i] = lsp_mid[i] * 0.5 + lsp_old[i] * 0.5 */

    for (i = 0; i < M; i++)
    {
        //lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_old[i], 1));
        lsp[i] = ADD((SHR_D(lsp_mid[i], 1)), (SHR_D(lsp_old[i], 1)));


    }
    Lsp_Az (lsp, Az);           /* Subframe 1 */
    Az += MP1 * 2;

    for (i = 0; i < M; i++)
    {
        //lsp[i] = add (shr (lsp_mid[i], 1), shr (lsp_new[i], 1));
        lsp[i] = ADD((SHR_D(lsp_mid[i], 1)), (SHR_D(lsp_new[i], 1)));

    }
    Lsp_Az (lsp, Az);           /* Subframe 3 */

    return;
}
