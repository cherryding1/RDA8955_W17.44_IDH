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

#include "bl_map.h"
#include "bl_platform.h"

BL_MAP_T gMapTable SECTION_SRAM_UCBSS;
BL_MAP_T *gMapTablePtr SECTION_FIXPTR;

void bl_map_init(void)
{
    for (int n = 0; n < BL_MAP_COUNT; n++)
    {
        gMapTable.modules[n].version = NULL;
        gMapTable.modules[n].access = NULL;
    }
    gMapTablePtr = &gMapTable;
}

void bl_map_register(int id, void *version, void *access)
{
    if (id >= 0 && id < BL_MAP_COUNT)
    {
        gMapTable.modules[id].version = version;
        gMapTable.modules[id].access = access;
    }
}
