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



#ifndef RM_MEMORY_DEFAULT_H
#define RM_MEMORY_DEFAULT_H

#include "helix_types.h"

INT32 rm_memory_default_init();
void* rm_memory_default_malloc(void* pUserMem, UINT32 ulSize);
void rm_memory_default_free(void* pUserMem, void* ptr);
void rm_memory_default_free_all();

#endif /* RM_MEMORY_DEFAULT_H */
