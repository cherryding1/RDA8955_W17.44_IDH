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






#include        "vpp_fr_config.h"

#ifdef  HAS_STDLIB_H
#include        <stdlib.h>
#else
#       include "vpp_fr_proto.h"
extern char     * memset P((char *, int, int));
#endif
#include <stdio.h>

#include "vpp_fr_gsm.h"
#include "vpp_fr_private.h"
#include "vpp_fr_proto.h"

#include "vpp_fr_vad.h"
#include "vpp_fr_dtx.h"
#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

struct gsm_state g_r;

void gsm_create P0()
{
    //gsm  r;

    VPP_FR_PROFILE_FUNCTION_ENTER(gsm_create);

#ifdef  USE_TABLE_MUL

    static int mul_init = 0;
    if (!mul_init)
    {
        mul_init = 1;
        init_umul_table();
    }

#endif

    //r = (gsm)malloc(sizeof(struct gsm_state));
    //if (!r) return r;

    memset(&g_r, 0, sizeof(g_r));
    g_r.nrp = 40;

    vad_init();//fts add
    dtx_init();//fts add

    VPP_FR_PROFILE_FUNCTION_EXIT(gsm_create);

}
