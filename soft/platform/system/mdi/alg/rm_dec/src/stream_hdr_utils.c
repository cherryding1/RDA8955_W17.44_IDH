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


#ifdef SHEEN_VC_DEBUG
#include <memory.h>
#endif
#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"
#include "pack_utils.h"
#include "stream_hdr_structs.h"
#include "stream_hdr_utils.h"

HX_RESULT rm_unpack_rule_map(BYTE**             ppBuf,
                             UINT32*            pulLen,
                             rm_malloc_func_ptr fpMalloc,
                             rm_free_func_ptr   fpFree,
                             void*              pUserMem,
                             rm_rule_map*       pMap)
{
    HX_RESULT retVal = HXR_FAIL;

    if (ppBuf && pulLen && fpMalloc && fpFree && pMap &&
            *ppBuf && *pulLen >= 2)
    {
        /* Initialize local variables */
        UINT32 ulSize = 0;
        UINT32 i      = 0;
        /* Clean up any existing rule to flag map */
        rm_cleanup_rule_map(fpFree, pUserMem, pMap);
        /* Unpack the number of rules */
        pMap->ulNumRules = rm_unpack16(ppBuf, pulLen);
        if (pMap->ulNumRules && *pulLen >= pMap->ulNumRules * 2)
        {
            /* Allocate the map array */
            ulSize = pMap->ulNumRules * sizeof(UINT32);
            pMap->pulMap = (UINT32*) fpMalloc(pUserMem, ulSize);
            if (pMap->pulMap)
            {
                /* Zero out the memory */
                memset(pMap->pulMap, 0, ulSize);
                /* Unpack each of the flags */
                for (i = 0; i < pMap->ulNumRules; i++)
                {
                    pMap->pulMap[i] = rm_unpack16(ppBuf, pulLen);
                }
                /* Clear the return value */
                retVal = HXR_OK;
            }
        }
        else
        {
            /* No rules - not an error */
            retVal = HXR_OK;
        }
    }

    return retVal;
}

void rm_cleanup_rule_map(rm_free_func_ptr fpFree,
                         void*            pUserMem,
                         rm_rule_map*     pMap)
{
    if (fpFree && pMap && pMap->pulMap)
    {
        fpFree(pUserMem, pMap->pulMap);
        pMap->pulMap     = (UINT32*)HXNULL;
        pMap->ulNumRules = 0;
    }
}

HX_RESULT rm_unpack_multistream_hdr(BYTE**              ppBuf,
                                    UINT32*             pulLen,
                                    rm_malloc_func_ptr  fpMalloc,
                                    rm_free_func_ptr    fpFree,
                                    void*               pUserMem,
                                    rm_multistream_hdr* hdr)
{
    HX_RESULT retVal = HXR_FAIL;

    if (ppBuf && pulLen && fpMalloc && fpFree && hdr &&
            *ppBuf && *pulLen >= 4)
    {
        /* Unpack the multistream members */
        hdr->ulID = rm_unpack32(ppBuf, pulLen);
        /* Unpack the rule to substream map */
        retVal = rm_unpack_rule_map(ppBuf, pulLen,
                                    fpMalloc, fpFree, pUserMem,
                                    &hdr->rule2SubStream);
        if (retVal == HXR_OK)
        {
            if (*pulLen >= 2)
            {
                /* Unpack the number of substreams */
                hdr->ulNumSubStreams = rm_unpack16(ppBuf, pulLen);
            }
            else
            {
                retVal = HXR_FAIL;
            }
        }
    }

    return retVal;
}

void rm_cleanup_multistream_hdr(rm_free_func_ptr    fpFree,
                                void*               pUserMem,
                                rm_multistream_hdr* hdr)
{
    if (hdr)
    {
        rm_cleanup_rule_map(fpFree, pUserMem, &hdr->rule2SubStream);
    }
}
