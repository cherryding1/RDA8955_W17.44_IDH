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





#include "string.h"


/*-
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 */

void *
memscan(s, c, n)
void *s;
int c;
size_t n;
{
    const unsigned char *p = s;

    while(n--)
    {
        if (*p++ ==c)
            return __UNCONST(p-1);
    }
    return __UNCONST(p);
}


