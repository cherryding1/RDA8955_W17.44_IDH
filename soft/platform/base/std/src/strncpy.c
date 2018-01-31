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

/*
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 */

/*
 * Copy src to dst, truncating or null-padding to always copy n bytes.
 * Return dst.
 */
char *
strncpy(dst, src, n)
char *dst;
const char *src;
size_t n;
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;

        do
        {
            if ((*d++ = *s++) == 0)
            {
                /* NUL pad the remaining n-1 bytes */
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        }
        while (--n != 0);
    }
    return (dst);
}


