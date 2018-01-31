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



#include <stddef.h>
#include <string.h>


/* Return the length of the maximum initial segment of S
   which contains no characters from REJECT.  */
size_t
strcspn (const char *s, const char *reject)
{
    size_t count = 0;

    while (*s != '\0')
        if (strchr (reject, *s++) == NULL)
            ++count;
        else
            return count;

    return count;
}

