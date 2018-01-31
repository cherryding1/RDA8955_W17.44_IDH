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

size_t
strlen(str)
const char *str;
{
    const char *s;

    for (s = str; *s; ++s);
    return(s - str);
}

size_t
strnlen(str, len)
const char *str;
size_t len;
{
    const char *s;

    for (s = str; len-- && *s; ++s);
    return(s - str);
}


