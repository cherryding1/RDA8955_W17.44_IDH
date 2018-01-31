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
#include <stdio.h>
#include <memory.h>
#endif
#include "helix_types.h"
#include "helix_result.h"
#include "rm_parse.h"
#include "rm_parser_internal.h"
#include "rm_memory_default.h"
#include "rm_error_default.h"
#include "rm_io_default.h"
#include "memory_utils.h"
#include "pack_utils.h"

/*
 * We will create a buffer of this size to handle
 * reading of headers. The buffer will get increased
 * in size if we encounter a header bigger than this
 * size. Therefore, we would like to pick a reasonable
 * size so that the we don't have lots of little
 * allocations. This size should be the maximum
 * of the expected size of the headers in the .rm file.
 *
 * XXXMEH - run lots of .rm files to see what a good
 * size to pick.
 */
#define RM_PARSER_INITIAL_READ_BUFFER_SIZE 256

HXBOOL rm_parser_is_rm_file(BYTE* pBuf, UINT32 ulSize)
{
    HXBOOL bRet = FALSE;

    /* Look for magic number ".RMF" at the beginning */
    if (ulSize >= 4)
    {
        UINT32 ulID = rm_unpack32(&pBuf, &ulSize);
        if (ulID == RM_HEADER_OBJECT)
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

rm_parser* rm_parser_create(void*             pError,
                            rm_error_func_ptr fpError)
{
    return rm_parser_create2(pError,
                             fpError,
                             HXNULL,
                             rm_memory_default_malloc,
                             rm_memory_default_free);
}

rm_parser* rm_parser_create2(void*              pError,
                             rm_error_func_ptr  fpError,
                             void*              pMem,
                             rm_malloc_func_ptr fpMalloc,
                             rm_free_func_ptr   fpFree)
{
    rm_parser* pRet = HXNULL;

    if (fpMalloc)
    {
        /* Allocate space for the rm_parser_internal struct
         * by using the passed-in malloc function
         */
        rm_parser_internal* pInt = (rm_parser_internal*) fpMalloc(pMem, sizeof(rm_parser_internal));
        if (pInt)
        {
            /* Zero out the struct */
            memset((void*) pInt, 0, sizeof(rm_parser_internal));
            /*
             * Assign the error members. If the user did not
             * provide an error callback, then use the default
             * rm_error_default().
             */
            if (fpError)
            {
                pInt->fpError    = fpError;
                pInt->pUserError = pError;
            }
            else
            {
                pInt->fpError    = rm_error_default;
                pInt->pUserError = HXNULL;
            }
            /* Assign the memory functions */
            pInt->fpMalloc = fpMalloc;
            pInt->fpFree   = fpFree;
            pInt->pUserMem = pMem;
            /* Assign the return value */
            pRet = (rm_parser*) pInt;
        }
    }

    return pRet;
}

HX_RESULT rm_parser_init_stdio(rm_parser* pParser,
                               FILE_HDL      fp)
{
    HX_RESULT retVal = HXR_FAIL;
#ifdef SHEEN_VC_DEBUG
    if (pParser && fp)
#else
    if (pParser)
#endif
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Assign the default io functions */
        pInt->fpRead    = rm_io_default_read;
        pInt->fpSeek    = rm_io_default_seek;
        pInt->pUserRead = (void*) fp;
        /* Clear the return value */
        retVal = HXR_OK;
    }

    return retVal;
}

HX_RESULT rm_parser_init_io(rm_parser*        pParser,
                            void*             pUserRead,
                            rm_read_func_ptr  fpRead,
                            rm_seek_func_ptr  fpSeek)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser && fpRead && fpSeek)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Assign the default io functions */
        pInt->fpRead    = fpRead;
        pInt->fpSeek    = fpSeek;
        pInt->pUserRead = pUserRead;
        /* Clear the return value */
        retVal = HXR_OK;
    }

    return retVal;
}

HX_RESULT rm_parser_read_headers(rm_parser* pParser)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Clear the return value */
        retVal = HXR_OK;
        /* Allocate the read buffer if necessary */
        if (!pInt->pReadBuffer)
        {
            retVal = rm_enforce_buffer_min_size(pInt->pUserMem,
                                                pInt->fpMalloc,
                                                pInt->fpFree,
                                                &pInt->pReadBuffer,
                                                &pInt->ulReadBufferSize,
                                                RM_PARSER_INITIAL_READ_BUFFER_SIZE);
            if (retVal == HXR_OK)
            {
                /* Set the number of bytes read to zero */
                pInt->ulNumBytesRead = 0;
            }
        }
        if (retVal == HXR_OK)
        {
            retVal = rm_parseri_read_all_headers(pInt);
        }
    }

    return retVal;
}


UINT32 rm_parser_get_max_bit_rate(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.max_bit_rate;
    }

    return ulRet;
}

UINT32 rm_parser_get_avg_bit_rate(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.avg_bit_rate;
    }

    return ulRet;
}

UINT32 rm_parser_get_max_packet_size(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.max_pkt_size;
    }

    return ulRet;
}

UINT32 rm_parser_get_avg_packet_size(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.avg_pkt_size;
    }

    return ulRet;
}

UINT32 rm_parser_get_num_packets(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.num_pkts;
    }

    return ulRet;
}

UINT32 rm_parser_get_duration(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.duration;
    }

    return ulRet;
}

UINT32 rm_parser_get_preroll(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->propHdr.preroll;
    }

    return ulRet;
}

const char* rm_parser_get_title(rm_parser* pParser)
{
    const char* pRet = HXNULL;

    if (pParser)
    {
        pRet = (const char*) ((rm_parser_internal*) pParser)->contHdr.title;
    }

    return pRet;
}

const char* rm_parser_get_author(rm_parser* pParser)
{
    const char* pRet = HXNULL;

    if (pParser)
    {
        pRet = (const char*) ((rm_parser_internal*) pParser)->contHdr.author;
    }

    return pRet;
}

const char* rm_parser_get_copyright(rm_parser* pParser)
{
    const char* pRet = HXNULL;

    if (pParser)
    {
        pRet = (const char*) ((rm_parser_internal*) pParser)->contHdr.copyright;
    }

    return pRet;
}

const char* rm_parser_get_comment(rm_parser* pParser)
{
    const char* pRet = HXNULL;

    if (pParser)
    {
        pRet = (const char*) ((rm_parser_internal*) pParser)->contHdr.comment;
    }

    return pRet;
}

UINT32 rm_parser_get_num_streams(rm_parser* pParser)
{
    UINT32 ulRet = 0;

    if (pParser)
    {
        ulRet = ((rm_parser_internal*) pParser)->ulNumStreams;
    }

    return ulRet;
}

HX_RESULT rm_parser_get_file_properties(rm_parser*    pParser,
                                        rm_property** ppProp,
                                        UINT32*       pulNumProps)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser && ppProp && pulNumProps)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        if (pInt->fpMalloc && pInt->fpFree)
        {
            /* Get the number of file properties */
            UINT32 ulNum = rm_parseri_get_num_file_properties(pInt);
            if (ulNum)
            {
                /* Compute the size of the rm_property array */
                UINT32 ulSize = ulNum * sizeof(rm_property);
                /* Allocate an array of this size */
                rm_property* pPropArr = pInt->fpMalloc(pInt->pUserMem, ulSize);
                if (pPropArr)
                {
                    /* NULL out the memory */
                    memset(pPropArr, 0, ulSize);
                    /* Collect the properties */
                    retVal = rm_parseri_get_file_properties(pInt, pPropArr, ulNum);
                    if (retVal == HXR_OK)
                    {
                        /* Assign the out parameters */
                        *ppProp      = pPropArr;
                        *pulNumProps = ulNum;
                    }
                    else
                    {
                        /* Free the array */
                        pInt->fpFree(pInt->pUserMem, pPropArr);
                    }
                }
            }
        }
    }

    return retVal;
}

void rm_parser_destroy_properties(rm_parser*    pParser,
                                  rm_property** ppProp,
                                  UINT32*       pulNumProps)
{
    if (pParser && ppProp && pulNumProps && *ppProp && *pulNumProps)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        if (pInt->fpFree)
        {
            /* Get the pointer to the array */
            rm_property* pProp = *ppProp;
            /* Clean up the properties */
            UINT32 i = 0;
            for (i = 0; i < *pulNumProps; i++)
            {
                rm_parseri_cleanup_rm_property(pInt, &pProp[i]);
            }
            /* Free the memory */
            pInt->fpFree(pInt->pUserMem, pProp);
            /* NULL out the pointer */
            *ppProp = HXNULL;
            /* Zero out the number of properties */
            *pulNumProps = 0;
        }
    }
}

HX_RESULT rm_parser_get_stream_header(rm_parser*         pParser,
                                      UINT32             ulStreamNum,
                                      rm_stream_header** ppHdr)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser && ppHdr)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        if (pInt->fpMalloc)
        {
            rm_stream_header* pHdr = (rm_stream_header*) pInt->fpMalloc(pInt->pUserMem,
                                     sizeof(rm_stream_header));
            if (pHdr)
            {
                /* NULL out the memory */
                memset(pHdr, 0, sizeof(rm_stream_header));
                /* Copy the stream header */
                retVal = rm_parseri_copy_stream_header(pInt, ulStreamNum, pHdr);
                if (retVal == HXR_OK)
                {
                    /* Assign the out parameter */
                    *ppHdr = pHdr;
                }
                else
                {
                    /* Clean up the stream header */
                    rm_parseri_cleanup_stream_header(pInt, pHdr);
                    /* Free the memory we allocated */
                    pInt->fpFree(pInt->pUserMem, pHdr);
                }
            }
        }
    }

    return retVal;
}

void rm_parser_destroy_stream_header(rm_parser*         pParser,
                                     rm_stream_header** ppHdr)
{
    if (pParser && ppHdr && *ppHdr)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Cleanup the stream header */
        rm_parseri_cleanup_stream_header(pInt, *ppHdr);
        /* Free the memory associated with this header */
        pInt->fpFree(pInt->pUserMem, *ppHdr);
        /* NULL out the pointer */
        *ppHdr = HXNULL;
    }
}

HX_RESULT rm_parser_get_packet(rm_parser*  pParser,
                               rm_packet** ppPacket,
                               UINT16 usStreamNum)//add usStreamNum for audio or video.sheen
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser && ppPacket)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Read the next packet */
        retVal = rm_parseri_read_next_packet(pInt, ppPacket, usStreamNum);
    }

    return retVal;
}

HX_RESULT rm_parser_get_packet2(rm_parser* pParser,
                                rm_packet* pPacket,
                                UINT16 usStreamNum)//add usStreamNum for audio or video.sheen
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Read the next packet */
        retVal = rm_parseri_read_next_packet2(pInt, pPacket, usStreamNum);
    }

    return retVal;
}

void rm_parser_destroy_packet(rm_parser*  pParser,
                              rm_packet** ppPacket)
{
    if (pParser && ppPacket)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Call the internal function to cleanup the packet's members */
        rm_parseri_cleanup_packet(pInt, *ppPacket);
        /* Do we have a free function pointer? */
        if (pInt->fpFree)
        {
            /* Free the rm_packet struct itself */
            pInt->fpFree(pInt->pUserMem, *ppPacket);
            /* Null out the pointer value */
            *ppPacket = HXNULL;
        }
    }
}

void rm_parser_cleanup_packet(rm_parser* pParser,
                              rm_packet* pPacket)
{
    if (pParser)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Call the internal function */
        rm_parseri_cleanup_packet(pInt, pPacket);
    }
}

HX_RESULT rm_parser_seek(rm_parser* pParser,
                         UINT32     ulTime)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pParser)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) pParser;
        /* Read the next packet */
        retVal = rm_parseri_seek(pInt, ulTime);
    }

    return retVal;
}

void rm_parser_destroy(rm_parser** ppParser)
{
    if (ppParser)
    {
        /* Get the internal parser struct */
        rm_parser_internal* pInt = (rm_parser_internal*) *ppParser;
        if (pInt && pInt->fpFree)
        {
            /* Save a pointer to fpFree and pUserMem */
            rm_free_func_ptr fpFree   = pInt->fpFree;
            void*            pUserMem = pInt->pUserMem;
            /* Clean up the content header */
            rm_parseri_cleanup_content_hdr(pInt);
            /* Clean up the media props headers */
            rm_parseri_cleanup_all_media_props_hdrs(pInt);
            /* Clean up all logical stream headers */
            rm_parseri_cleanup_all_logical_stream_hdrs(pInt);
            /* Clean up the logical fileinfo header */
            rm_parseri_cleanup_logical_fileinfo_hdr(pInt);
            /* Clean up any stream-level logical-fileinfo headers */
            rm_parseri_cleanup_stream_logical_fileinfo_hdrs(pInt);
            /* Clean up the read buffer */
            rm_parseri_cleanup_read_buffer(pInt);
            /* Free the stream num map */
            rm_parseri_cleanup_stream_num_map(pInt);
            /* Clean up the stream info array */
            rm_parseri_cleanup_stream_info_array(pInt);
            /* Clean up the stream header array */
            rm_parseri_cleanup_all_stream_headers(pInt);
            /* Clean up the allocator */
            rm_allocator_cleanup(&pInt->allocator);
            /* Null everything out */
            memset(pInt, 0, sizeof(rm_parser_internal));
            /* Free the rm_parser_internal struct memory */
            fpFree(pUserMem, pInt);
            /* NULL out the pointer */
            *ppParser = HXNULL;
        }
    }
}

