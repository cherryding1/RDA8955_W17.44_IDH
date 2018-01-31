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

INT16 Enc_lag6 (      /* output: Return index of encoding     */
    INT16 T0,         /* input : Pitch delay                  */
    INT16 *T0_frac,   /* in/out: Fractional pitch delay       */
    INT16 *T0_min,    /* in/out: Minimum search delay         */
    INT16 *T0_max,    /* in/out: Maximum search delay         */
    INT16 pit_min,    /* input : Minimum pitch delay          */
    INT16 pit_max,    /* input : Maximum pitch delay          */
    INT16 pit_flag    /* input : Flag for 1st or 3rd subframe */
)
{
    INT16 index, i;
    VPP_EFR_PROFILE_FUNCTION_ENTER(Enc_lag6);

    if (pit_flag == 0)          /* if 1st or 3rd subframe */
    {
        /* encode pitch delay (with fraction) */


        //if (sub (T0, 94) <= 0)
        if (SUB (T0, 94) <= 0)
        {
            /* index = T0*6 - 105 + *T0_frac */
            //i = add (add (T0, T0), T0);
            i = ADD (ADD (T0, T0), T0);
            //index = add (sub (add (i, i), 105), *T0_frac);
            index = ADD (SUB (ADD (i, i), 105), *T0_frac);
        }
        else
        {
            /* set fraction to 0 for delays > 94 */
            *T0_frac = 0;
            //index = add (T0, 368);
            index = ADD (T0, 368);
        }

        /* find T0_min and T0_max for second (or fourth) subframe */

        //*T0_min = sub (T0, 5);
        *T0_min = SUB (T0, 5);



        //if (sub (*T0_min, pit_min) < 0)
        if (SUB (*T0_min, pit_min) < 0)
        {
            *T0_min = pit_min;
        }

        //*T0_max = add (*T0_min, 9);
        *T0_max = ADD (*T0_min, 9);


        //if (sub (*T0_max, pit_max) > 0)
        if (SUB (*T0_max, pit_max) > 0)
        {
            *T0_max = pit_max;
            //*T0_min = sub (*T0_max, 9);
            *T0_min = SUB (*T0_max, 9);

        }
    }
    else
        /* if second or fourth subframe */
    {
        /* index = 6*(T0-*T0_min) + 3 + *T0_frac  */
        //i = sub (T0, *T0_min);
        i = SUB (T0, *T0_min);
        //i = add (add (i, i), i);
        i = ADD (ADD (i, i), i);

        //index = add (add (add (i, i), 3), *T0_frac);
        index = ADD (ADD (ADD (i, i), 3), *T0_frac);

    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Enc_lag6);
    return index;
}
