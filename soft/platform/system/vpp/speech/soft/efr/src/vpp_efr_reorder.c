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

void Reorder_lsf (
    INT16 *lsf,        /* (i/o)     : vector of LSFs   (range: 0<=val<=0.5) */
    INT16 min_dist,    /* (i)       : minimum required distance             */
    INT16 n            /* (i)       : LPC order                             */
)
{
    INT16 i;
    INT16 lsf_min;

    lsf_min = min_dist;
    for (i = 0; i < n; i++)
    {

        //if (sub (lsf[i], lsf_min) < 0)
        if (lsf[i]<lsf_min)
        {
            lsf[i] = lsf_min;
        }
        //lsf_min = add (lsf[i], min_dist);
        lsf_min = ADD (lsf[i], min_dist);

    }
}





















