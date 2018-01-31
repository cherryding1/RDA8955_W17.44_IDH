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



#ifndef PACK_UTILS_H
#define PACK_UTILS_H

#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"

/* Pack a 32-bit value big-endian */
void rm_pack32(UINT32 ulValue, BYTE** ppBuf, UINT32* pulLen);
/* Pack a 32-bit value little-endian */
void rm_pack32_le(UINT32 ulValue, BYTE** ppBuf, UINT32* pulLen);
/* Pack a 16-bit value big-endian */
void rm_pack16(UINT16 usValue, BYTE** ppBuf, UINT32* pulLen);
/* Pack a 16-bit value little-endian */
void rm_pack16_le(UINT16 usValue, BYTE** ppBuf, UINT32* pulLen);
void rm_pack8(BYTE ucValue, BYTE** ppBuf, UINT32* pulLen);

/* Unpacking utilties */
UINT32    rm_unpack32(BYTE** ppBuf, UINT32* pulLen);
UINT16    rm_unpack16(BYTE** ppBuf, UINT32* pulLen);
/* rm_unpack32() and rm_unpack16() have the side
 * effect of incrementing *ppBuf and decrementing
 * *pulLen. The functions below (rm_unpack32_nse()
 * rm_unpack16_nse()) do not change pBuf and ulLen.
 * That is, they have No Side Effect, hence the suffix
 * _nse.
 */
UINT32    rm_unpack32_nse(BYTE* pBuf, UINT32 ulLen);
UINT16    rm_unpack16_nse(BYTE* pBuf, UINT32 ulLen);
BYTE      rm_unpack8(BYTE** ppBuf, UINT32* pulLen);
HX_RESULT rm_unpack_string(BYTE**             ppBuf,
                           UINT32*            pulLen,
                           UINT32             ulStrLen,
                           char**             ppStr,
                           void*              pUserMem,
                           rm_malloc_func_ptr fpMalloc,
                           rm_free_func_ptr   fpFree);
HX_RESULT rm_unpack_buffer(BYTE**             ppBuf,
                           UINT32*            pulLen,
                           UINT32             ulBufLen,
                           BYTE**             ppUnPackBuf,
                           void*              pUserMem,
                           rm_malloc_func_ptr fpMalloc,
                           rm_free_func_ptr   fpFree);
HX_RESULT rm_unpack_array(BYTE**             ppBuf,
                          UINT32*            pulLen,
                          UINT32             ulNumElem,
                          UINT32             ulElemSize,
                          void**             ppArr,
                          void*              pUserMem,
                          rm_malloc_func_ptr fpMalloc,
                          rm_free_func_ptr   fpFree);

UINT32 rm_unpack32_from_byte_string(BYTE** ppBuf, UINT32* pulLen);

#endif /* #ifndef PACK_UTILS_H */
