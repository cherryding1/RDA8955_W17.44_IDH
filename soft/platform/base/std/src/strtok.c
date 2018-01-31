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

/* Strtok. Ugly. Bug-prone. Hey, this is C, after all */
char *
strtok(char *s, const char *delim)
{
    static char *lasts;

    return strtok_r(s, delim, &lasts);
}

/* Reentrant version */
char *
strtok_r(char *s, const char *delim, char **lasts)
{
    const char *spanp;
    int c, sc;
    char *tok;

    /* s may be NULL */

    if (s == NULL && (s = *lasts) == NULL)
        return (NULL);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = delim; (sc = *spanp++) != 0;)
    {
        if (c == sc)
            goto cont;
    }

    if (c == 0)          /* no non-delimiter characters */
    {
        *lasts = NULL;
        return (NULL);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
    {
        c = *s++;
        spanp = delim;
        do
        {
            if ((sc = *spanp++) == c)
            {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *lasts = s;
                return (tok);
            }
        }
        while (sc != 0);
    }
    /* NOTREACHED */
}
