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
#include "ra_depack.h"
#include "ra_depack_internal.h"
#include "rm_memory_default.h"
#include "rm_error_default.h"
#include "memory_utils.h"

ra_depack* ra_depack_create(void*                   pAvail,
                            ra_block_avail_func_ptr fpAvail,
                            void*                   pUserError,
                            rm_error_func_ptr       fpError)
{
    return ra_depack_create2(pAvail,
                             fpAvail,
                             pUserError,
                             fpError,
                             HXNULL,
                             rm_memory_default_malloc,
                             rm_memory_default_free);
}

ra_depack* ra_depack_create2(void*                   pAvail,
                             ra_block_avail_func_ptr fpAvail,
                             void*                   pUserError,
                             rm_error_func_ptr       fpError,
                             void*                   pUserMem,
                             rm_malloc_func_ptr      fpMalloc,
                             rm_free_func_ptr        fpFree)
{
    ra_depack* pRet = HXNULL;

    if (fpAvail && fpMalloc && fpFree)
    {
        /* Allocate space for the ra_depack_internal struct
         * by using the passed-in malloc function
         */
        ra_depack_internal* pInt =
            (ra_depack_internal*) fpMalloc(pUserMem, sizeof(ra_depack_internal));
        if (pInt)
        {
            /* Zero out the struct */
            memset((void*) pInt, 0, sizeof(ra_depack_internal));
            /* Assign the frame callback members */
            pInt->pAvail  = pAvail;
            pInt->fpAvail = fpAvail;
            /*
             * Assign the error members. If the caller did not
             * provide an error callback, then use the default
             * rm_error_default().
             */
            if (fpError)
            {
                pInt->fpError    = fpError;
                pInt->pUserError = pUserError;
            }
            else
            {
                pInt->fpError    = rm_error_default;
                pInt->pUserError = HXNULL;
            }
            /* Assign the memory functions */
            pInt->fpMalloc = fpMalloc;
            pInt->fpFree   = fpFree;
            pInt->pUserMem = pUserMem;
            /* Assign the return value */
            pRet = (ra_depack*) pInt;
        }
    }

    return pRet;
}

HX_RESULT ra_depack_init(ra_depack* pDepack, rm_stream_header* header)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pDepack && header)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Call the internal init */
        retVal = ra_depacki_init(pInt, header);
    }

    return retVal;
}

UINT32 ra_depack_get_num_substreams(ra_depack* pDepack)
{
    UINT32 ulRet = 0;

    if (pDepack)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Return the number of substreams */
        ulRet = pInt->multiStreamHdr.ulNumSubStreams;
    }

    return ulRet;
}

UINT32 ra_depack_get_codec_4cc(ra_depack* pDepack, UINT32 ulSubStream)
{
    UINT32 ulRet = 0;

    if (pDepack)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Make sure the substream index is legal */
        if (pInt->pSubStreamHdr &&
                ulSubStream < pInt->multiStreamHdr.ulNumSubStreams)
        {
            ulRet = pInt->pSubStreamHdr[ulSubStream].ulCodecID;
        }
    }

    return ulRet;
}

//add sheen
BYTE ra_depack_get_codec_aac_type(ra_depack* pDepack, UINT32 ulSubStream)
{
    UINT32 ulRet = 0;

    if (pDepack)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Make sure the substream index is legal */
        if (pInt->pSubStreamHdr &&
                ulSubStream < pInt->multiStreamHdr.ulNumSubStreams)
        {
            ulRet = pInt->pSubStreamHdr[ulSubStream].ucStreamType;
        }
    }

    return ulRet;
}

HX_RESULT ra_depack_get_codec_init_info(ra_depack*       pDepack,
                                        UINT32           ulSubStream,
                                        ra_format_info** ppInfo)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pDepack && ppInfo)
    {
        /* Init local variables */
        UINT32          ulSize = sizeof(ra_format_info);
        ra_format_info* pInfo  = HXNULL;
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Allocate space for the struct */
        pInfo = ra_depacki_malloc(pInt, ulSize);
        if (pInfo)
        {
            /* NULL out the memory */
            memset(pInfo, 0, ulSize);
            /* Fill in the init info struct */
            retVal = ra_depacki_get_format_info(pInt, ulSubStream, pInfo);
            if (retVal == HXR_OK)
            {
                /* Assign the out parameter */
                *ppInfo = pInfo;
            }
            else
            {
                /* We failed so free the memory we allocated */
                ra_depacki_free(pInt, pInfo);
            }
        }
    }

    return retVal;
}

void ra_depack_destroy_codec_init_info(ra_depack* pDepack, ra_format_info** ppInfo)
{
    if (pDepack && ppInfo && *ppInfo)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Clean up the format info struct */
        ra_depacki_cleanup_format_info(pInt, *ppInfo);
        /* Delete the memory associated with it */
        ra_depacki_free(pInt, *ppInfo);
        /* NULL the pointer out */
        *ppInfo = HXNULL;
    }
}

HX_RESULT ra_depack_add_packet(ra_depack* pDepack, rm_packet* packet)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pDepack && packet)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Call the internal function */
        retVal = ra_depacki_add_packet(pInt, packet);
    }

    return retVal;
}

void ra_depack_destroy_block(ra_depack* pDepack, ra_block** ppBlock)
{
    if (pDepack && ppBlock && *ppBlock)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Call the internal function */
        ra_depacki_destroy_block(pInt, ppBlock);
    }
}

HX_RESULT ra_depack_seek(ra_depack* pDepack, UINT32 ulTime)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pDepack)
    {
        /* Get the internal struct */
        ra_depack_internal* pInt = (ra_depack_internal*) pDepack;
        /* Call the internal seek function */
        retVal = ra_depacki_seek(pInt, ulTime);
    }

    return retVal;
}

void ra_depack_destroy(ra_depack** ppDepack)
{
    if (ppDepack)
    {
        ra_depack_internal* pInt = (ra_depack_internal*) *ppDepack;
        if (pInt && pInt->fpFree)
        {
            /* Save a pointer to fpFree and pUserMem */
            rm_free_func_ptr fpFree   = pInt->fpFree;
            void*            pUserMem = pInt->pUserMem;
            /* Free the rule-to-flag map (if present) */
            if (pInt->rule2Flag.pulMap)
            {
                fpFree(pUserMem, pInt->rule2Flag.pulMap);
                pInt->rule2Flag.pulMap = HXNULL;
            }
            /* Clean up the rule to header map */
            if (pInt->multiStreamHdr.rule2SubStream.pulMap)
            {
                ra_depacki_free(pInt, pInt->multiStreamHdr.rule2SubStream.pulMap);
                pInt->multiStreamHdr.rule2SubStream.pulMap     = HXNULL;
                pInt->multiStreamHdr.rule2SubStream.ulNumRules = 0;
            }
            /* Clean up the substream header array */
            ra_depacki_cleanup_substream_hdr_array(pInt);
            /* Null everything out */
            memset(pInt, 0, sizeof(ra_depack_internal));
            /* Free the rm_parser_internal struct memory */
            fpFree(pUserMem, pInt);
            /* NULL out the pointer */
            *ppDepack = HXNULL;
        }
    }
}

