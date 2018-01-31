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

#include "vpp_efr_lag_wind.tab"

void Lag_window (
    INT16 m,           /* (i)     : LPC order                        */
    INT16 r_h[],       /* (i/o)   : Autocorrelations  (msb)          */
    INT16 r_l[]        /* (i/o)   : Autocorrelations  (lsb)          */
)
{
    INT16 i;
    INT32 x;

    for (i = 1; i <= m; i++)
    {
        x = Mpy_32 (r_h[i], r_l[i], lag_h[i - 1], lag_l[i - 1]);
        L_Extract (x, &r_h[i], &r_l[i]);
    }
    return;
}
