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

/* Old integer lag */

INT16 old_T0;

INT16
Dec_lag6 (             /* output: return integer pitch lag       */
    INT16 index,      /* input : received pitch index           */
    INT16 pit_min,    /* input : minimum pitch lag              */
    INT16 pit_max,    /* input : maximum pitch lag              */
    INT16 i_subfr,    /* input : subframe flag                  */
    INT16 L_frame_by2,/* input : speech frame size divided by 2 */
    INT16 *T0_frac,   /* output: fractional part of pitch lag   */
    INT16 bfi         /* input : bad frame indicator            */
)
{
    INT16 pit_flag;
    INT16 T0, i;
    static INT16 T0_min, T0_max;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Dec_lag6);

    pit_flag = i_subfr;         /* flag for 1st or 3rd subframe */

    //if (sub (i_subfr, L_frame_by2) == 0)
    if (SUB (i_subfr, L_frame_by2) == 0)
    {
        pit_flag = 0;
    }

    if (pit_flag == 0)          /* if 1st or 3rd subframe */
    {

        if (bfi == 0)
        {
            /* if bfi == 0 decode pitch */

            //if (sub (index, 463) < 0)
            if (SUB (index, 463) < 0)
            {
                /* T0 = (index+5)/6 + 17 */
                //T0 = add (mult (add(index, 5), 5462), 17);
                T0 = ADD (MULT (ADD(index, 5), 5462), 17);

                //i = add (add (T0, T0), T0);
                i = ADD (ADD (T0, T0), T0);

                /* *T0_frac = index - T0*6 + 105 */
                //*T0_frac = add (sub (index, add(i, i)), 105);
                *T0_frac = ADD (SUB (index, ADD(i, i)), 105);

            }
            else
            {
                // T0 = sub (index, 368);
                T0 = SUB (index, 368);
                *T0_frac = 0;
            }
        }
        else
            /* bfi == 1 */
        {
            T0 = old_T0;
            *T0_frac = 0;
        }

        /* find T0_min and T0_max for 2nd (or 4th) subframe */

        //T0_min = sub (T0, 5);
        T0_min = SUB (T0, 5);

        //if (sub (T0_min, pit_min) < 0)
        if (SUB (T0_min, pit_min) < 0)
        {
            T0_min = pit_min;
        }

        //T0_max = add(T0_min, 9);
        T0_max = ADD(T0_min, 9);

        //if (sub (T0_max, pit_max) > 0)
        if (SUB (T0_max, pit_max) > 0)
        {
            T0_max = pit_max;
            //T0_min = sub (T0_max, 9);
            T0_min = SUB (T0_max, 9);
        }
    }
    else
        /* second or fourth subframe */
    {
        /* if bfi == 0 decode pitch */
        //if ((bfi == 0) && (sub (index, 61) < 0))
        if ((bfi == 0) && (SUB (index, 61) < 0))
        {
            /* i = (index+5)/6 - 1 */
            //i = sub (mult (add (index, 5), 5462), 1);
            i = SUB (MULT (ADD (index, 5), 5462), 1);
            // T0 = add (i, T0_min);
            T0 = ADD (i, T0_min);
            //i = add (add (i, i), i);
            i = ADD (ADD (i, i), i);
            //*T0_frac = sub (sub (index, 3), add (i, i));
            *T0_frac = SUB (SUB (index, 3), ADD (i, i));

        }
        else
            /* bfi == 1  OR index >= 61 */
        {
            T0 = old_T0;
            *T0_frac = 0;
        }
    }

    old_T0 = T0;
    VPP_EFR_PROFILE_FUNCTION_EXIT(Dec_lag6);

    return T0;
}





















