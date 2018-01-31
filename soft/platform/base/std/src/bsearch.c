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



#include "stddef.h"
#include <stdlib.h>


/* Perform a binary search for KEY in BASE which has NMEMB elements
   of SIZE bytes each.  The comparisons are done by (*COMPAR)().  */
void *
bsearch (const void *key, const void *base, size_t nmemb, size_t size,
         int (*compar) __P ((const void *, const void *)))
{
    size_t l, u, idx;
    const void *p;
    int comparison;

    l = 0;
    u = nmemb;
    while (l < u)
    {
        idx = (l + u) / 2;
        p = (void *) (((const char *) base) + (idx * size));
        comparison = (*compar) (key, p);
        if (comparison < 0)
            u = idx;
        else if (comparison > 0)
            l = idx + 1;
        else
            return (void *) p;
    }

    return NULL;
}

