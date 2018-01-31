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



#include <string.h>
#include "string_utils.h"

char* copy_string(void*              pUserMem,
                  rm_malloc_func_ptr fpMalloc,
                  const char*        pszStr)
{
    char* pRet = (char*)HXNULL;

    if (fpMalloc && pszStr)
    {
        /* Allocate space for string */
        pRet = (char*) fpMalloc(pUserMem, strlen(pszStr) + 1);
        if (pRet)
        {
            /* Copy the string */
            strcpy(pRet, pszStr);
        }
    }

    return pRet;
}

void free_string(void*            pUserMem,
                 rm_free_func_ptr fpFree,
                 char**           ppszStr)
{
    if (fpFree && ppszStr && *ppszStr)
    {
        fpFree(pUserMem, *ppszStr);
        *ppszStr = (char*)HXNULL;
    }
}

#if defined(_WINDOWS)

#if !defined(_WINCE)

int strcasecmp(const char* pszStr1, const char* pszStr2)
{
    return _stricmp(pszStr1, pszStr2);
}

#endif /* #if !defined(_WINCE) */

int strncasecmp(const char* pszStr1, const char* pszStr2, int len)
{
    return _strnicmp(pszStr1, pszStr2, (size_t) len);
}

#endif /* #if defined(_WINDOWS) */

