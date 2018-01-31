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

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

void Copy (
    INT16 x[],         /* (i)   : input vector   */
    INT16 y[],         /* (o)   : output vector  */
    INT16 L            /* (i)   : vector length  */
)
{
    INT16 i;
    VPP_EFR_PROFILE_FUNCTION_ENTER(Copy);

    for (i = 0; i < L; i++)
    {
        y[i] = x[i];
    }

    VPP_EFR_PROFILE_FUNCTION_EXIT(Copy);
    return;
}
