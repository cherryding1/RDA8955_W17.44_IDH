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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "cs_types.h"
#include "memd_m.h"
#include "memdp.h"


PRIVATE UINT32 memd_FlashGetSectorLimits(UINT32 sectorId, UINT32 *start, UINT32 *end)
{
    UINT32 aStart, aEnd;
    int bankNum = 0;
    int majSecIdx, minSecIdx;
    UINT32 sectSize;

    if (sectorId == g_memdFlashLayout.fullSize)
    {
        *start = g_memdFlashLayout.fullSize;
        *end = g_memdFlashLayout.fullSize;
        return 0;
    }
    else if (sectorId > g_memdFlashLayout.fullSize)
    {
        *start = 0;
        *end = 0;
        fprintf(stderr, "Out of accessible flash space !!!!");
        return 0;
    }
    else
    {
        while ( (sectorId >= g_memdFlashLayout.bankLayout[bankNum+1].bankStartAddr) &&
                (bankNum < (g_memdFlashLayout.numberOfBank-1)) )
        {
            ++bankNum;
        }
        aStart = g_memdFlashLayout.bankLayout[bankNum].bankStartAddr;

        majSecIdx = 0;
        aEnd = aStart +
               (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);
        while (sectorId >= aEnd)
        {
            if (majSecIdx == 2)
            {
                *start = 0;
                *end = 0;
                fprintf(stderr, "Out of accessible flash space !!!!");
                return 0;
            } else
            {
                ++majSecIdx;
                aStart = aEnd;
                aEnd = aStart +
                       (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                        g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);
            }
        }

        minSecIdx = 0;
        sectSize = g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1];
        aEnd = aStart + sectSize;
        while (sectorId >= aEnd)
        {
            aStart = aEnd;
            aEnd = aStart + sectSize;
            ++minSecIdx;
        }

        *start = aStart;
        *end = aEnd;
        return sectSize;
    }
}

// =============================================================================
// memd_FlashGetUserFieldInfo
// -----------------------------------------------------------------------------
/// That function get the user data info from memd.def.
///
/// @param userFieldBase The USER_DATA_BASE defined in memd.def.
/// @param userFieldSize The USER_DATA_SIZE defined in memd.def.
/// @param sectorSize The sector size of user data field, it is the large sector size.
///        Some small boot sectors can be regarded as one large sector.
/// @return VOID.
// =============================================================================
PUBLIC VOID memd_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize)
{
    UINT32 Start, End;

    *userFieldBase = g_memdFlashLayout.userDataBase;
    *userFieldSize = g_memdFlashLayout.userDataSize;
    *sectorSize = memd_FlashGetSectorLimits(*userFieldBase, &Start, &End);
    *blockSize = g_memdFlashLayout.userDataSectorLogicalBlockSize;
}
