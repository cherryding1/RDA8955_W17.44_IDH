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





#include <stdlib.h>
//#else
//#include "hlxclib/stdlib.h"
//#endif

#include "aac_coder.h"
#ifndef SHEEN_VC_DEBUG
#include "mmc.h"
#endif

/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
void ClearBuffer(void *buf, int nBytes)
{
    int i;
    unsigned char *cbuf = (unsigned char *)buf;

    for (i = 0; i < nBytes; i++)
        cbuf[i] = 0;

    return;
}

/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the AAC decoder
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to AACDecInfo structure, cleared to all 0's (except for
 *                pointer to platform-specific data structure)
 *
 * Notes:       if one or more mallocs fail, function frees any buffers already
 *                allocated before returning
 **************************************************************************************/
AACDecInfo *AllocateBuffers(void)
{
    AACDecInfo *aacDecInfo;
#ifdef SHEEN_VC_DEBUG
    aacDecInfo = (AACDecInfo *)malloc(sizeof(AACDecInfo));
#else
    aacDecInfo = (AACDecInfo *)mmc_MemMalloc(sizeof(AACDecInfo));
#endif
    if (!aacDecInfo)
        return 0;
    ClearBuffer(aacDecInfo, sizeof(AACDecInfo));
#ifdef SHEEN_VC_DEBUG
    aacDecInfo->psInfoBase = malloc(sizeof(PSInfoBase));
#else
    aacDecInfo->psInfoBase = (void*)mmc_MemMalloc(sizeof(PSInfoBase));
#endif
    if (!aacDecInfo->psInfoBase)
    {
        FreeBuffers(aacDecInfo);
        return 0;
    }
    ClearBuffer(aacDecInfo->psInfoBase, sizeof(PSInfoBase));

    return aacDecInfo;
}

#ifndef SAFE_FREE
#ifdef SHEEN_VC_DEBUG
#define SAFE_FREE(x)    {if (x) free(x);    (x) = 0;}   /* helper macro */
#else
#define SAFE_FREE(x)
#endif
#endif

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: frees all the memory used by the AAC decoder
 *
 * Inputs:      pointer to initialized AACDecInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       safe to call even if some buffers were not allocated (uses SAFE_FREE)
 **************************************************************************************/
void FreeBuffers(AACDecInfo *aacDecInfo)
{
    if (!aacDecInfo)
        return;

    SAFE_FREE(aacDecInfo->psInfoBase);
    SAFE_FREE(aacDecInfo);
}
