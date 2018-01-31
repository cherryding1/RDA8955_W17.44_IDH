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

#include <string.h>

size_t
strspn(const char *s1, const char *s2)
{
    const char *p = s1, *spanp;
    char c, sc;

cont:
    c = *p++;
    for (spanp = s2; (sc = *spanp++) != 0;)
        if (sc == c)
            goto cont;
    return (p - 1 - s1);
}

