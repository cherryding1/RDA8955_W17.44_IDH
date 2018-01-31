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

INT16 mem_pre;

void preemphasis (
    INT16 *signal, /* (i/o)   : input signal overwritten by the output */
    INT16 g,       /* (i)     : preemphasis coefficient                */
    INT16 L        /* (i)     : size of filtering                      */
)
{
    INT16 *p1, *p2, temp, i;

    p1 = signal + L - 1;
    p2 = p1 - 1;
    temp = *p1;

    for (i = 0; i <= L - 2; i++)
    {
        //*p1 = sub (*p1, mult (g, *p2--));
        *p1 = SUB (*p1, MULT(g, *p2--));

        p1--;
    }

    //*p1 = sub (*p1, mult (g, mem_pre));
    *p1 = SUB (*p1, MULT(g, mem_pre));


    mem_pre = temp;

    return;
}
