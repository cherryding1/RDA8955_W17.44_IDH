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
#include <stdio.h>

#define L_CODE    40            /* codevector length */
#define NB_PULSE  10            /* number of pulses  */
#define NB_TRACK  5             /* number of track */

void dec_10i40_35bits (
    INT16 index[],    /* (i)     : index of 10 pulses (sign+position)       */
    INT16 cod[]       /* (o)     : algebraic (fixed) codebook excitation    */
)
{
    static const INT16 dgray[8] = {0, 1, 3, 2, 5, 6, 4, 7};
    INT16 i, j, pos1, pos2, sign, tmp;

    VPP_EFR_PROFILE_FUNCTION_ENTER(dec_10i40_35bits);


    for (i = 0; i < L_CODE; i++)
    {
        cod[i] = 0;
    }

    /* decode the positions and signs of pulses and build the codeword */

    for (j = 0; j < NB_TRACK; j++)
    {
        /* compute index i */

        tmp = index[j];
        i = tmp & 7;
        i = dgray[i];

        //i = extract_l (L_shr (L_mult (i, 5), 1));
        i = EXTRACT_L (L_SHR_D(L_MULT (i, 5), 1));

        //pos1 = add(i, j); /* position of pulse "j" */
        pos1 = ADD(i, j);

        //i = shr (tmp, 3) & 1;
        i = SHR_D(tmp, 3) & 1;

        if (i == 0)
        {
            sign = 4096;
        }
        else
        {
            sign = -4096;
        }

        cod[pos1] = sign;

        /* compute index i */

        //i = index[add(j, 5)] & 7;
        i = index[ADD(j, 5)] & 7;


        i = dgray[i];
        //i = extract_l (L_shr (L_mult (i, 5), 1));
        i = EXTRACT_L (L_SHR_D(L_MULT(i, 5), 1));



        //pos2 = add(i, j);      /* position of pulse "j+5" */
        pos2 = ADD(i, j);

        //if (sub (pos2, pos1) < 0)
        if (SUB (pos2, pos1) < 0)
        {
            // sign = negate (sign);
            sign = NEGATE(sign);
        }

        //cod[pos2] = add(cod[pos2], sign);
        cod[pos2] = ADD(cod[pos2], sign);
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(dec_10i40_35bits);

    return;
}





















