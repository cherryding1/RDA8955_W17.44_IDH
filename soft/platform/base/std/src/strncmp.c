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


int
strncmp(s1, s2, n)
const char *s1, *s2;
size_t n;
{

    if (n == 0)
        return (0);
    do
    {
        if (*s1 != *s2++)
            return (*(const unsigned char *)s1 -
                    *(const unsigned char *)--s2);
        if (*s1++ == 0)
            break;
    }
    while (--n != 0);
    return (0);
}


