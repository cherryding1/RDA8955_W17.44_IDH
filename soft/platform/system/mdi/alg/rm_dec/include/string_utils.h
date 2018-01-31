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



#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "helix_types.h"
#include "rm_memory.h"
/* Create and copy a string */
char* copy_string(void*              pUserMem,
                  rm_malloc_func_ptr fpMalloc,
                  const char*        pszStr);

/* Free a string */
void free_string(void*            pUserMem,
                 rm_free_func_ptr fpFree,
                 char**           ppszStr);

#if defined(_WINDOWS)

int strcasecmp(const char* pszStr1, const char* pszStr2);
int strncasecmp(const char* pszStr1, const char* pszStr2, int len);

#elif defined(_UNIX)
#include <strings.h>
#endif

#endif /* #ifndef STRING_UTILS_H */
