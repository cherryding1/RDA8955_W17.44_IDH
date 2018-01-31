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
#include "memory_utils.h"

HX_RESULT rm_enforce_buffer_min_size(void*              pUserMem,
                                     rm_malloc_func_ptr fpMalloc,
                                     rm_free_func_ptr   fpFree,
                                     BYTE**             ppBuf,
                                     UINT32*            pulCurLen,
                                     UINT32             ulReqLen)
{
    HX_RESULT retVal = HXR_OUTOFMEMORY;

    if (fpMalloc && fpFree && ppBuf && pulCurLen)
    {
        if (ulReqLen > *pulCurLen)
        {
            /* Allocate a new buffer */
            BYTE* pBuf = (BYTE*)fpMalloc(pUserMem, ulReqLen);
            if (pBuf)
            {
                /* Was the old buffer allocated? */
                if (*ppBuf && *pulCurLen > 0)
                {
                    /* Copy the old buffer into the new one */
                    memcpy(pBuf, *ppBuf, *pulCurLen);
                    /* Free the old buffer */
                    fpFree(pUserMem, *ppBuf);
                }
                /* Assign the buffer out parameter */
                *ppBuf = pBuf;
                /* Change the current size to the requested size */
                *pulCurLen = ulReqLen;
                /* Clear the return value */
                retVal = HXR_OK;
            }
        }
        else
        {
            /* Current buffer size is adequate - don't
             * have to do anything here.
             */
            retVal = HXR_OK;
        }
    }

    return retVal;
}

BYTE* copy_buffer(void*              pUserMem,
                  rm_malloc_func_ptr fpMalloc,
                  BYTE*              pBuf,
                  UINT32             ulLen)
{
    BYTE* pRet = (BYTE*)HXNULL;

    if (fpMalloc && pBuf && ulLen)
    {
        /* Allocate a buffer */
        pRet = (BYTE*) fpMalloc(pUserMem, ulLen);
        if (pRet)
        {
            /* Copy the buffer */
            memcpy(pRet, pBuf, ulLen);
        }
    }

    return pRet;
}
