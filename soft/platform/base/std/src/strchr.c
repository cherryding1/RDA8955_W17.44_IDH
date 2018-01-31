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

char *
strchr(p, ch)
const char *p;
int ch;
{
    for (;; ++p)
    {
        if (*p == (char) ch)
            return __UNCONST(p);
        if (!*p)
            return NULL;
    }
    /* NOTREACHED */
}

char *
strrchr(p, ch)
const char *p;
int ch;
{
    char *save;

    for (save = NULL;; ++p)
    {
        if (*p == ch)
            save = __UNCONST(p);
        if (!*p)
            return(save);
    }
    /* NOTREACHED */
}


