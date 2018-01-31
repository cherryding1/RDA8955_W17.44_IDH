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



#include "helix_types.h"
#include "helix_mime_types.h"
//#include "helix_string.h"
#include "rm_property.h"
#include "rm_stream.h"

UINT32 rm_stream_get_number(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulStreamNumber;
    }

    return ulRet;
}

UINT32 rm_stream_get_max_bit_rate(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulMaxBitRate;
    }

    return ulRet;
}

UINT32 rm_stream_get_avg_bit_rate(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulAvgBitRate;
    }

    return ulRet;
}

UINT32 rm_stream_get_max_packet_size(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulMaxPacketSize;
    }

    return ulRet;
}

UINT32 rm_stream_get_avg_packet_size(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulAvgPacketSize;
    }

    return ulRet;
}

UINT32 rm_stream_get_start_time(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulStartTime;
    }

    return ulRet;
}

UINT32 rm_stream_get_preroll(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulPreroll;
    }

    return ulRet;
}

UINT32 rm_stream_get_duration(rm_stream_header* hdr)
{
    UINT32 ulRet = 0;

    if (hdr)
    {
        ulRet = hdr->ulDuration;
    }

    return ulRet;
}

const char* rm_stream_get_name(rm_stream_header* hdr)
{
    const char* pRet = (const char*)HXNULL;

    if (hdr)
    {
        pRet = (const char*) hdr->pStreamName;
    }

    return pRet;
}

const char* rm_stream_get_mime_type(rm_stream_header* hdr)
{
    const char* pRet = (const char*)HXNULL;

    if (hdr)
    {
        pRet = (const char*) hdr->pMimeType;
    }

    return pRet;
}

UINT32 rm_stream_get_properties(rm_stream_header* hdr, rm_property** ppProp)
{
    UINT32 ulRet = 0;

    if (hdr && ppProp)
    {
        *ppProp = hdr->pProperty;
        ulRet   = hdr->ulNumProperties;
    }

    return ulRet;
}

HXBOOL rm_stream_is_realaudio(rm_stream_header* hdr)
{
    HXBOOL bRet = FALSE;

    if (hdr)
    {
        bRet = rm_stream_is_realaudio_mimetype((const char*) hdr->pMimeType);
    }

    return bRet;
}

HXBOOL rm_stream_is_realvideo(rm_stream_header* hdr)
{
    HXBOOL bRet = FALSE;

    if (hdr)
    {
        bRet = rm_stream_is_realvideo_mimetype((const char*) hdr->pMimeType);
    }

    return bRet;
}

HXBOOL rm_stream_is_realevent(rm_stream_header* hdr)
{
    HXBOOL bRet = FALSE;

    if (hdr)
    {
        bRet = rm_stream_is_realevent_mimetype((const char*) hdr->pMimeType);
    }

    return bRet;
}

HXBOOL rm_stream_is_realaudio_mimetype(const char* pszStr)
{
    HXBOOL bRet = FALSE;

    if (pszStr)
    {
        if (!strcmp(pszStr, REALAUDIO_MIME_TYPE)           ||
                !strcmp(pszStr, REALAUDIO_ENCRYPTED_MIME_TYPE) ||
                !strcmp(pszStr, REALAUDIO_LOSSLESS_MIME_TYPE))
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

HXBOOL rm_stream_is_realaudio_lossless_mimetype(const char* pszStr)
{
    HXBOOL bRet = FALSE;

    if (pszStr)
    {
        if (!strcmp(pszStr, REALAUDIO_LOSSLESS_MIME_TYPE))
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

HXBOOL rm_stream_is_realvideo_mimetype(const char* pszStr)
{
    HXBOOL bRet = FALSE;

    if (pszStr)
    {
        if (!strcmp(pszStr, REALVIDEO_MIME_TYPE) ||
                !strcmp(pszStr, REALVIDEO_ENCRYPTED_MIME_TYPE))
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

HXBOOL rm_stream_is_realevent_mimetype(const char* pszStr)
{
    HXBOOL bRet = FALSE;

    if (pszStr)
    {
        if (!strcmp(pszStr, REALEVENT_MIME_TYPE)              ||
                !strcmp(pszStr, REALEVENT_ENCRYPTED_MIME_TYPE)    ||
                !strcmp(pszStr, REALIMAGEMAP_MIME_TYPE)           ||
                !strcmp(pszStr, REALIMAGEMAP_ENCRYPTED_MIME_TYPE) ||
                !strcmp(pszStr, IMAGEMAP_MIME_TYPE)               ||
                !strcmp(pszStr, IMAGEMAP_ENCRYPTED_MIME_TYPE)     ||
                !strcmp(pszStr, SYNCMM_MIME_TYPE)                 ||
                !strcmp(pszStr, SYNCMM_ENCRYPTED_MIME_TYPE))
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

HXBOOL rm_stream_is_real_mimetype(const char* pszStr)
{
    return rm_stream_is_realaudio_mimetype(pszStr) ||
           rm_stream_is_realvideo_mimetype(pszStr) ||
           rm_stream_is_realevent_mimetype(pszStr);
}

HX_RESULT rm_stream_get_property_int(rm_stream_header* hdr,
                                     const char*       pszStr,
                                     UINT32*           pulVal)
{
    HX_RESULT retVal = HXR_FAIL;

    if (hdr && pszStr && pulVal &&
            hdr->pProperty && hdr->ulNumProperties)
    {
        UINT32 i = 0;
        for (i = 0; i < hdr->ulNumProperties; i++)
        {
            rm_property* pProp = &hdr->pProperty[i];
            if (pProp->ulType == RM_PROPERTY_TYPE_UINT32 &&
                    pProp->pName                             &&
                    !strcmp(pszStr, (const char*) pProp->pName))
            {
                /* Assign the out parameter */
                *pulVal = (UINT32) pProp->pValue;
                /* Clear the return value */
                retVal = HXR_OK;
                break;
            }
        }
    }

    return retVal;
}

HX_RESULT rm_stream_get_property_buf(rm_stream_header* hdr,
                                     const char*       pszStr,
                                     BYTE**            ppBuf,
                                     UINT32*           pulLen)
{
    HX_RESULT retVal = HXR_FAIL;

    if (hdr && pszStr && ppBuf && pulLen &&
            hdr->pProperty && hdr->ulNumProperties)
    {
        UINT32 i = 0;
        for (i = 0; i < hdr->ulNumProperties; i++)
        {
            rm_property* pProp = &hdr->pProperty[i];
            if (pProp->ulType == RM_PROPERTY_TYPE_BUFFER &&
                    pProp->pName                             &&
                    !strcmp(pszStr, (const char*) pProp->pName))
            {
                /* Assign the out parameters */
                *ppBuf  = pProp->pValue;
                *pulLen = pProp->ulValueLen;
                /* Clear the return value */
                retVal = HXR_OK;
                break;
            }
        }
    }

    return retVal;
}

HX_RESULT rm_stream_get_property_str(rm_stream_header* hdr,
                                     const char*       pszStr,
                                     char**            ppszStr)
{
    HX_RESULT retVal = HXR_FAIL;

    if (hdr && pszStr && ppszStr &&
            hdr->pProperty && hdr->ulNumProperties)
    {
        UINT32 i = 0;
        for (i = 0; i < hdr->ulNumProperties; i++)
        {
            rm_property* pProp = &hdr->pProperty[i];
            if (pProp->ulType == RM_PROPERTY_TYPE_CSTRING &&
                    pProp->pName                              &&
                    !strcmp(pszStr, (const char*) pProp->pName))
            {
                /* Assign the out parameter */
                *ppszStr = (char*) pProp->pValue;
                /* Clear the return value */
                retVal = HXR_OK;
                break;
            }
        }
    }

    return retVal;
}
