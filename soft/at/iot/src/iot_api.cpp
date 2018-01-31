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

#include "iot_api.h"
#include "cs_types.h"

void *operator new(size_t sz)
{
    IOT_trace("------COS_MALLOC--------");
    return (void *)COS_MALLOC(sz);
}

void operator delete(void *p)
{
    IOT_trace("------COS_FREE--------");
    if (p)
        COS_FREE(p);
}
