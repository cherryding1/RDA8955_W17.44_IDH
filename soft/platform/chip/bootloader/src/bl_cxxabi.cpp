/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

extern "C" {
#include <stdint.h>
#include <bl_platform.h>
#include <bl_malloc.h>
}

void* operator new(uint32_t size)
{
    void* ptr = bl_ddr_malloc(size);
    return ptr;
}

void operator delete(void *ptr)
{
    if (ptr != NULL)
        bl_free(ptr);
}

void *operator new[](uint32_t size)
{
    void *ptr = bl_ddr_malloc(size);
    return ptr;
}

void operator delete[](void *ptr)
{
    if (ptr != NULL)
        bl_free(ptr);
}

extern "C" void __cxa_pure_virtual()
{
    bl_panic();
}
