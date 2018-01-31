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

void rm_pack32(UINT32 ulValue, BYTE** ppBuf, UINT32* pulLen)
{
    if (ppBuf && pulLen && *pulLen >= 4)
    {
        BYTE* pBuf = *ppBuf;
        pBuf[0] = (BYTE) ((ulValue & 0xFF000000) >> 24);
        pBuf[1] = (BYTE) ((ulValue & 0x00FF0000) >> 16);
        pBuf[2] = (BYTE) ((ulValue & 0x0000FF00) >>  8);
        pBuf[3] = (BYTE)  (ulValue & 0x000000FF);
        *ppBuf  += 4;
        *pulLen -= 4;
    }
}

void rm_pack32_le(UINT32 ulValue, BYTE** ppBuf, UINT32* pulLen)
{
    if (ppBuf && pulLen && *pulLen >= 4)
    {
        BYTE* pBuf = *ppBuf;
        pBuf[0] = (BYTE)  (ulValue & 0x000000FF);
        pBuf[1] = (BYTE) ((ulValue & 0x0000FF00) >>  8);
        pBuf[2] = (BYTE) ((ulValue & 0x00FF0000) >> 16);
        pBuf[3] = (BYTE) ((ulValue & 0xFF000000) >> 24);
        *ppBuf  += 4;
        *pulLen -= 4;
    }
}

void rm_pack16(UINT16 usValue, BYTE** ppBuf, UINT32* pulLen)
{
    if (ppBuf && pulLen && *pulLen >= 2)
    {
        BYTE* pBuf = *ppBuf;
        pBuf[0] = (BYTE) ((usValue & 0x0000FF00) >> 8);
        pBuf[1] = (BYTE)  (usValue & 0x000000FF);
        *ppBuf  += 2;
        *pulLen -= 2;
    }
}

void rm_pack16_le(UINT16 usValue, BYTE** ppBuf, UINT32* pulLen)
{
    if (ppBuf && pulLen && *pulLen >= 2)
    {
        BYTE* pBuf = *ppBuf;
        pBuf[0] = (BYTE)  (usValue & 0x000000FF);
        pBuf[1] = (BYTE) ((usValue & 0x0000FF00) >> 8);
        *ppBuf  += 2;
        *pulLen -= 2;
    }
}

void rm_pack8(BYTE ucValue, BYTE** ppBuf, UINT32* pulLen)
{
    if (ppBuf && pulLen && *pulLen > 0)
    {
        BYTE* pBuf = *ppBuf;
        pBuf[0] = (BYTE) ucValue;
        *ppBuf  += 1;
        *pulLen -= 1;
    }
}

UINT32 rm_unpack32(BYTE** ppBuf, UINT32* pulLen)
{
    UINT32 ulRet = 0;

    if (ppBuf && pulLen && *pulLen >= 4)
    {
        BYTE* pBuf = *ppBuf;
        ulRet      = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];
        *ppBuf    += 4;
        *pulLen   -= 4;
    }

    return ulRet;
}

UINT16 rm_unpack16(BYTE** ppBuf, UINT32* pulLen)
{
    UINT16 usRet = 0;

    if (ppBuf && pulLen && *pulLen >= 2)
    {
        BYTE* pBuf = *ppBuf;
        usRet      = (pBuf[0] << 8) | pBuf[1];
        *ppBuf    += 2;
        *pulLen   -= 2;
    }

    return usRet;
}

UINT32 rm_unpack32_nse(BYTE* pBuf, UINT32 ulLen)
{
    UINT32 ulRet = 0;

    if (pBuf && ulLen >= 4)
    {
        ulRet = (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3];
    }

    return ulRet;
}

UINT16 rm_unpack16_nse(BYTE* pBuf, UINT32 ulLen)
{
    UINT16 usRet = 0;

    if (pBuf && ulLen >= 2)
    {
        usRet = (pBuf[0] << 8) | pBuf[1];
    }

    return usRet;
}

BYTE rm_unpack8(BYTE** ppBuf, UINT32* pulLen)
{
    BYTE ucRet = 0;

    if (ppBuf && pulLen && *pulLen > 0)
    {
        BYTE* pBuf = *ppBuf;
        ucRet      = pBuf[0];
        *ppBuf    += 1;
        *pulLen   -= 1;
    }

    return ucRet;
}

HX_RESULT rm_unpack_string(BYTE**             ppBuf,
                           UINT32*            pulLen,
                           UINT32             ulStrLen,
                           char**             ppStr,
                           void*              pUserMem,
                           rm_malloc_func_ptr fpMalloc,
                           rm_free_func_ptr   fpFree)
{
    HX_RESULT retVal = HXR_FAIL;

    if (ppBuf && pulLen && *pulLen >= ulStrLen && ppStr && fpMalloc && fpFree)
    {
        /* Clear the return value */
        retVal = HXR_OK;
        /* Do we actually have a string to unpack? */
        if (ulStrLen)
        {
            /* If the string is already allocated, free it */
            if (*ppStr)
            {
                fpFree(pUserMem, *ppStr);
                *ppStr = (char*)HXNULL;
            }
            /* Allocate a buffer that it one more byte than the length */
            *ppStr = (char*) fpMalloc(pUserMem, ulStrLen + 1);
            if (*ppStr)
            {
                /* Copy the string buffer in */
                memcpy(*ppStr, *ppBuf, ulStrLen);
                /* Put a NULL terminator on the end */
                (*ppStr)[ulStrLen] = '\0';
                /* Update the parsing buffer counters */
                *ppBuf  += ulStrLen;
                *pulLen -= ulStrLen;
            }
            else
            {
                retVal = HXR_OUTOFMEMORY;
            }
        }
    }

    return retVal;
}

HX_RESULT rm_unpack_buffer(BYTE**             ppBuf,
                           UINT32*            pulLen,
                           UINT32             ulBufLen,
                           BYTE**             ppUnPackBuf,
                           void*              pUserMem,
                           rm_malloc_func_ptr fpMalloc,
                           rm_free_func_ptr   fpFree)
{
    HX_RESULT retVal = HXR_FAIL;

    if (ppBuf && pulLen && *pulLen >= ulBufLen && ppUnPackBuf && fpMalloc && fpFree)
    {
        /* Clear the return value */
        retVal = HXR_OK;
        /* Do we have a buffer? */
        if (ulBufLen)
        {
            /* If the string is already allocated, free it */
            if (*ppUnPackBuf)
            {
                fpFree(pUserMem, *ppUnPackBuf);
                *ppUnPackBuf = (BYTE*)HXNULL;
            }
            /* Allocate a buffer that it one more byte than the length */
            *ppUnPackBuf = (BYTE*) fpMalloc(pUserMem, ulBufLen);
            if (*ppUnPackBuf)
            {
                /* Copy the string buffer in */
                memcpy(*ppUnPackBuf, *ppBuf, ulBufLen);
                /* Update the parsing buffer counters */
                *ppBuf  += ulBufLen;
                *pulLen -= ulBufLen;
            }
            else
            {
                retVal = HXR_OUTOFMEMORY;
            }
        }
    }

    return retVal;
}

HX_RESULT rm_unpack_array(BYTE**             ppBuf,
                          UINT32*            pulLen,
                          UINT32             ulNumElem,
                          UINT32             ulElemSize,
                          void**             ppArr,
                          void*              pUserMem,
                          rm_malloc_func_ptr fpMalloc,
                          rm_free_func_ptr   fpFree)
{
    HX_RESULT retVal = HXR_FAIL;

    if (ppBuf && pulLen && *pulLen >= ulNumElem * ulElemSize &&
            ppArr && fpMalloc && fpFree)
    {
        /* Clear the return value */
        retVal = HXR_OK;
        /* Do we have any elements? */
        if (ulNumElem)
        {
            /* If the array is already allocated, then free it */
            if (*ppArr)
            {
                fpFree(pUserMem, *ppArr);
                *ppArr = HXNULL;
            }
            /* Allocate space for the array */
            *ppArr = fpMalloc(pUserMem, ulNumElem * ulElemSize);
            if (*ppArr)
            {
                /* Is this a UINT32 or a UINT16? */
                UINT32 i = 0;
                if (ulElemSize == sizeof(UINT32))
                {
                    /* Unpack UINT32s */
                    UINT32* pArr32 = (UINT32*) *ppArr;
                    for (i = 0; i < ulNumElem; i++)
                    {
                        pArr32[i] = rm_unpack32(ppBuf, pulLen);
                    }
                }
                else if (ulElemSize == sizeof(UINT16))
                {
                    /* Unpack UINT16s */
                    UINT16* pArr16 = (UINT16*) *ppArr;
                    for (i = 0; i < ulNumElem; i++)
                    {
                        pArr16[i] = rm_unpack16(ppBuf, pulLen);
                    }
                }
                /* Clear the return value */
                retVal = HXR_OK;
            }
            else
            {
                retVal = HXR_OUTOFMEMORY;
            }
        }
    }

    return retVal;
}

UINT32 rm_unpack32_from_byte_string(BYTE** ppBuf, UINT32* pulLen)
{
    UINT32 ulRet = 0;

    if (ppBuf && *ppBuf && pulLen && *pulLen)
    {
        UINT32 ulStrLen = rm_unpack8(ppBuf, pulLen);
        if (ulStrLen == 4 && *pulLen >= ulStrLen)
        {
            ulRet = rm_unpack32(ppBuf, pulLen);
        }
    }

    return ulRet;
}
