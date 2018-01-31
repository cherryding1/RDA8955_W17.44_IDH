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







#include "vpp_fr_private.h"

#include "vpp_fr_gsm.h"
#include "vpp_fr_proto.h"
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

int gsm_option P3((r, opt, val), gsm r, long opt, long * val)
{
    int     result = -1;

    VPP_FR_PROFILE_FUNCTION_ENTER(gsm_option);

    switch (opt)
    {
        case GSM_OPT_VERBOSE:
#ifndef NDEBUG
            result = r->verbose;
            if (val) r->verbose = (char) (*val);
#endif
            break;

        case GSM_OPT_FAST:

#if     defined(FAST) && defined(USE_FLOAT_MUL)
            result = r->fast;
            if (val) r->fast = !!*val;
#endif
            break;

        default:
            break;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(gsm_option);
    return result;
}
