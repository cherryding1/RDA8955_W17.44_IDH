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
#include "mp3_coder.h"

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
static void ClearBuffer(void *buf, int nBytes)
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
 * Description: allocate all the memory needed for the MP3 decoder
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      pointer to MP3DecInfo structure (initialized with pointers to all
 *                the internal buffers needed for decoding, all other members of
 *                MP3DecInfo structure set to 0)
 *
 * Notes:       if one or more mallocs fail, function frees any buffers already
 *                allocated before returning
 **************************************************************************************/
MP3DecInfo *AllocateBuffers(void)
{
    MP3DecInfo *mp3DecInfo;
    FrameHeader *fh;
    SideInfo *si;
    ScaleFactorInfo *sfi;
    HuffmanInfo *hi;
    DequantInfo *di;
    IMDCTInfo *mi;
    SubbandInfo *sbi;
#ifdef SHEEN_VC_DEBUG
    mp3DecInfo = (MP3DecInfo *)malloc(sizeof(MP3DecInfo));
#else
    mp3DecInfo = (MP3DecInfo *)mmc_MemMalloc(sizeof(MP3DecInfo));
#endif
    if (!mp3DecInfo)
        return 0;
    ClearBuffer(mp3DecInfo, sizeof(MP3DecInfo));
#ifdef SHEEN_VC_DEBUG
    fh =  (FrameHeader *)     malloc(sizeof(FrameHeader));
    si =  (SideInfo *)        malloc(sizeof(SideInfo));
    sfi = (ScaleFactorInfo *) malloc(sizeof(ScaleFactorInfo));
    hi =  (HuffmanInfo *)     malloc(sizeof(HuffmanInfo));
    di =  (DequantInfo *)     malloc(sizeof(DequantInfo));
    mi =  (IMDCTInfo *)       malloc(sizeof(IMDCTInfo));
    sbi = (SubbandInfo *)     malloc(sizeof(SubbandInfo));
#else
    fh =  (FrameHeader *)     mmc_MemMalloc(sizeof(FrameHeader));
    si =  (SideInfo *)        mmc_MemMalloc(sizeof(SideInfo));
    sfi = (ScaleFactorInfo *) mmc_MemMalloc(sizeof(ScaleFactorInfo));
    hi =  (HuffmanInfo *)     mmc_MemMalloc(sizeof(HuffmanInfo));
    di =  (DequantInfo *)     mmc_MemMalloc(sizeof(DequantInfo));
    mi =  (IMDCTInfo *)       mmc_MemMalloc(sizeof(IMDCTInfo));
    sbi = (SubbandInfo *)     mmc_MemMalloc(sizeof(SubbandInfo));
#endif

    mp3DecInfo->FrameHeaderPS =     (void *)fh;
    mp3DecInfo->SideInfoPS =        (void *)si;
    mp3DecInfo->ScaleFactorInfoPS = (void *)sfi;
    mp3DecInfo->HuffmanInfoPS =     (void *)hi;
    mp3DecInfo->DequantInfoPS =     (void *)di;
    mp3DecInfo->IMDCTInfoPS =       (void *)mi;
    mp3DecInfo->SubbandInfoPS =     (void *)sbi;

    if (!fh || !si || !sfi || !hi || !di || !mi || !sbi)
    {
        FreeBuffers(mp3DecInfo);    /* safe to call - only frees memory that was successfully allocated */
        return 0;
    }

    /* important to do this - DSP primitives assume a bunch of state variables are 0 on first use */
    ClearBuffer(fh,  sizeof(FrameHeader));
    ClearBuffer(si,  sizeof(SideInfo));
    ClearBuffer(sfi, sizeof(ScaleFactorInfo));
    ClearBuffer(hi,  sizeof(HuffmanInfo));
    ClearBuffer(di,  sizeof(DequantInfo));
    ClearBuffer(mi,  sizeof(IMDCTInfo));
    ClearBuffer(sbi, sizeof(SubbandInfo));

    return mp3DecInfo;
}

#ifdef SHEEN_VC_DEBUG
#define SAFE_FREE(x)    {if (x) free(x);    (x) = 0;}   /* helper macro */
#else
#define SAFE_FREE(x)
#endif

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: frees all the memory used by the MP3 decoder
 *
 * Inputs:      pointer to initialized MP3DecInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       safe to call even if some buffers were not allocated (uses SAFE_FREE)
 **************************************************************************************/
void FreeBuffers(MP3DecInfo *mp3DecInfo)
{
    if (!mp3DecInfo)
        return;

    SAFE_FREE(mp3DecInfo->FrameHeaderPS);
    SAFE_FREE(mp3DecInfo->SideInfoPS);
    SAFE_FREE(mp3DecInfo->ScaleFactorInfoPS);
    SAFE_FREE(mp3DecInfo->HuffmanInfoPS);
    SAFE_FREE(mp3DecInfo->DequantInfoPS);
    SAFE_FREE(mp3DecInfo->IMDCTInfoPS);
    SAFE_FREE(mp3DecInfo->SubbandInfoPS);

    SAFE_FREE(mp3DecInfo);
}
