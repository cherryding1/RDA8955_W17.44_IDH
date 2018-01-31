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



#ifndef PARSE_SDP_CHUNK_H
#define PARSE_SDP_CHUNK_H

#include "helix_types.h"
#include "helix_result.h"
#include "rm_property.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

UINT32    determine_property_type(const char* pszStr, UINT32 ulLen);
HX_RESULT parse_sdp_chunk(const char*         pszStr,
                          rm_malloc_func_ptr  fpMalloc,
                          rm_free_func_ptr    fpFree,
                          void*               pUserMem,
                          UINT32*             pulNumProp,
                          rm_property*        pProp);
HX_RESULT parse_property_set_from_sdp_chunk(const char*         pszStr,
        rm_malloc_func_ptr  fpMalloc,
        rm_free_func_ptr    fpFree,
        void*               pUserMem,
        rm_property_set*    pSet);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef PARSE_SDP_CHUNK_H */
