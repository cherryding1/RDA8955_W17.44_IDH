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

typedef char reg_char;

char *
strncat (s1, s2, n)
char *s1;
const char *s2;
size_t n;
{
    reg_char c;
    char *s = s1;

    /* Find the end of S1.  */
    do
        c = *s1++;
    while (c != '\0');

    /* Make S1 point before next character, so we can increment
       it while memory is read (wins on pipelined cpus).  */
    s1 -= 2;

    if (n >= 4)
    {
        size_t n4 = n >> 2;
        do
        {
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
            c = *s2++;
            *++s1 = c;
            if (c == '\0')
                return s;
        }
        while (--n4 > 0);
        n &= 3;
    }

    while (n > 0)
    {
        c = *s2++;
        *++s1 = c;
        if (c == '\0')
            return s;
        n--;
    }

    if (c != '\0')
        *++s1 = '\0';

    return s;
}

