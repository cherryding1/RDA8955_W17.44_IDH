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

/*
 * Convert a string to an unsigned long integer.
 */

unsigned long
strtoul(const char *nptr, char **endptr, int base)
{
    const char *s;
    unsigned long acc, cutoff;
    int c;
    int neg, any, cutlim;

    /*
     * See strtol for comments as to the logic used.
     */
    s = nptr;
    do
    {
        c = (unsigned char) *s++;
    }
    while (isspace(c));
    if (c == '-')
    {
        neg = 1;
        c = *s++;
    }
    else
    {
        neg = 0;
        if (c == '+')
            c = *s++;
    }
    if ((base == 0 || base == 16) &&
            c == '0' && (*s == 'x' || *s == 'X'))
    {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    cutoff = ULONG_MAX / (unsigned long)base;
    cutlim = (int)(ULONG_MAX % (unsigned long)base);
    for (acc = 0, any = 0;; c = (unsigned char) *s++)
    {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0)
            continue;
        if (acc > cutoff || (acc == cutoff && c > cutlim))
        {
            any = -1;
            acc = ULONG_MAX;
        }
        else
        {
            any = 1;
            acc *= (unsigned long)base;
            acc += c;
        }
    }
    if (neg && any > 0)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *)(any ? s - 1 : nptr);
    return (acc);
}

