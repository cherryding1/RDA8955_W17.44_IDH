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



#ifndef RA_BACKEND_H__
#define RA_BACKEND_H__

/* Unified RealAudio decoder backend interface */

#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"
#include "ra_format_info.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/* ra decoder backend interface */

typedef HX_RESULT (*ra_decode_init_func_ptr)(void*              pInitParams,
        UINT32             ulInitParamsSize,
        ra_format_info*    pStreamInfo,
        void**             pDecode,
        void*              pUserMem,
        rm_malloc_func_ptr fpMalloc,
        rm_free_func_ptr   fpFree);
typedef HX_RESULT (*ra_decode_reset_func_ptr)(void*   pDecode,
        UINT16* pSamplesOut,
        UINT32  ulNumSamplesAvail,
        UINT32* pNumSamplesOut);
typedef HX_RESULT (*ra_decode_conceal_func_ptr)(void*  pDecode,
        UINT32 ulNumSamples);
typedef HX_RESULT (*ra_decode_decode_func_ptr)(void*       pDecode,
        UINT8*      pData,
        UINT32      ulNumBytes,
        UINT32*     pNumBytesConsumed,
        UINT16*     pSamplesOut,
        UINT32      ulNumSamplesAvail,
        UINT32*     pNumSamplesOut,
        UINT32      ulFlags);
typedef HX_RESULT (*ra_decode_getmaxsize_func_ptr)(void*   pDecode,
        UINT32* pNumSamples);
typedef HX_RESULT (*ra_decode_getchannels_func_ptr)(void*   pDecode,
        UINT32* pNumChannels);
typedef HX_RESULT (*ra_decode_getchannelmask_func_ptr)(void*   pDecode,
        UINT32* pChannelMask);
typedef HX_RESULT (*ra_decode_getrate_func_ptr)(void*   pDecode,
        UINT32* pSampleRate);
typedef HX_RESULT (*ra_decode_getdelay_func_ptr)(void*   pDecode,
        UINT32* pNumSamples);
typedef HX_RESULT (*ra_decode_close_func_ptr)(void*            pDecode,
        void*            pUserMem,
        rm_free_func_ptr fpFree);

#ifdef __cplusplus
}
#endif

#endif /* RA_BACKEND_H__ */
