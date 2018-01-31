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





#include "ctype.h"
#include "stddef.h"


int atox(const char *s, int len)
{
    int i = 0;
    int j=0;

    for(j=0; j<len; j++)
    {
        int tmp = 0;
        if (*s >= '0' && *s <= '9')
            tmp = (*s++) - '0';
        else if (*s >= 'a' && *s <= 'f')
            tmp = (*s++) - 'a' + 10;
        else if (*s >= 'A' && *s <= 'F')
            tmp = (*s++) - 'A' + 10;
        else
            break;
        i = i * 16 + tmp;
    }

    return i;
}


