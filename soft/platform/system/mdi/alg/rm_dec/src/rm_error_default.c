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



#include <stdio.h>
#include "helix_types.h"
#include "rm_error.h"
#include "rm_error_default.h"

void rm_error_default(void* pUserError, HX_RESULT err, const char* msg)
{
    if (msg)
    {
        rm_printf("rm_error_default(pUserError=0x%08x,err=0x%08x,msg=%s\n",
                  pUserError, err, msg);
    }
    else
    {
        rm_printf( "rm_error_default(pUserError=0x%08x,err=0x%08x\n",
                   pUserError, err);
    }
}
