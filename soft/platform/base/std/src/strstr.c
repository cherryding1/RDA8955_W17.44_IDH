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

/*
 * Find the first occurrence of find in s.
 */
char *
strstr(s, find)
const char *s, *find;
{
    char c, sc;
    size_t len;

    if ((c = *find++) != 0)
    {
        len = strlen(find);
        do
        {
            do
            {
                if ((sc = *s++) == 0)
                    return (char *)0;
            }
            while (sc != c);
        }
        while (strncmp(s, find, len) != 0);
        s--;
    }
    return __UNCONST(s);
}


