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





#include "aac_coder.h"

/**************************************************************************************
 * Function:    DecodeSingleChannelElement
 *
 * Description: decode one SCE
 *
 * Inputs:      BitStreamInfo struct pointing to start of SCE (14496-3, table 4.4.4)
 *
 * Outputs:     updated element instance tag
 *
 * Return:      0 if successful, -1 if error
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
 **************************************************************************************/
static int DecodeSingleChannelElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
{
    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return -1;

    /* read instance tag */
    aacDecInfo->currInstTag = AacGetBits(bsi, NUM_INST_TAG_BITS);

    return 0;
}

/**************************************************************************************
 * Function:    DecodeChannelPairElement
 *
 * Description: decode one CPE
 *
 * Inputs:      BitStreamInfo struct pointing to start of CPE (14496-3, table 4.4.5)
 *
 * Outputs:     updated element instance tag
 *              updated commonWin
 *              updated ICS info, if commonWin == 1
 *              updated mid-side stereo info, if commonWin == 1
 *
 * Return:      0 if successful, -1 if error
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
 **************************************************************************************/
static int DecodeChannelPairElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
{
    int sfb, gp, maskOffset;
    unsigned char currBit, *maskPtr;
    PSInfoBase *psi;
    ICSInfo *icsInfo;

    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return -1;
    psi = (PSInfoBase *)(aacDecInfo->psInfoBase);
    icsInfo = psi->icsInfo;

    /* read instance tag */
    aacDecInfo->currInstTag = AacGetBits(bsi, NUM_INST_TAG_BITS);

    /* read common window flag and mid-side info (if present)
     * store msMask bits in psi->msMaskBits[] as follows:
     *  long blocks -  pack bits for each SFB in range [0, maxSFB) starting with lsb of msMaskBits[0]
     *  short blocks - pack bits for each SFB in range [0, maxSFB), for each group [0, 7]
     * msMaskPresent = 0 means no M/S coding
     *               = 1 means psi->msMaskBits contains 1 bit per SFB to toggle M/S coding
     *               = 2 means all SFB's are M/S coded (so psi->msMaskBits is not needed)
     */
    psi->commonWin = AacGetBits(bsi, 1);
    if (psi->commonWin)
    {
        DecodeICSInfo(bsi, icsInfo, psi->sampRateIdx);
        psi->msMaskPresent = AacGetBits(bsi, 2);
        if (psi->msMaskPresent == 1)
        {
            maskPtr = psi->msMaskBits;
            *maskPtr = 0;
            maskOffset = 0;
            for (gp = 0; gp < icsInfo->numWinGroup; gp++)
            {
                for (sfb = 0; sfb < icsInfo->maxSFB; sfb++)
                {
                    currBit = (unsigned char)AacGetBits(bsi, 1);
                    *maskPtr |= currBit << maskOffset;
                    if (++maskOffset == 8)
                    {
                        maskPtr++;
                        *maskPtr = 0;
                        maskOffset = 0;
                    }
                }
            }
        }
    }

    return 0;
}

/**************************************************************************************
 * Function:    DecodeLFEChannelElement
 *
 * Description: decode one LFE
 *
 * Inputs:      BitStreamInfo struct pointing to start of LFE (14496-3, table 4.4.9)
 *
 * Outputs:     updated element instance tag
 *
 * Return:      0 if successful, -1 if error
 *
 * Notes:       doesn't decode individual channel stream (part of DecodeNoiselessData)
 **************************************************************************************/
static int DecodeLFEChannelElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
{
    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return -1;

    /* read instance tag */
    aacDecInfo->currInstTag = AacGetBits(bsi, NUM_INST_TAG_BITS);

    return 0;
}

/**************************************************************************************
 * Function:    DecodeDataStreamElement
 *
 * Description: decode one DSE
 *
 * Inputs:      BitStreamInfo struct pointing to start of DSE (14496-3, table 4.4.10)
 *
 * Outputs:     updated element instance tag
 *              filled in data stream buffer
 *
 * Return:      0 if successful, -1 if error
 **************************************************************************************/
static int DecodeDataStreamElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
{
    unsigned int byteAlign, dataCount;
    unsigned char *dataBuf;
    PSInfoBase *psi;

    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return -1;
    psi = (PSInfoBase *)(aacDecInfo->psInfoBase);

    aacDecInfo->currInstTag = AacGetBits(bsi, NUM_INST_TAG_BITS);
    byteAlign = AacGetBits(bsi, 1);
    dataCount = AacGetBits(bsi, 8);
    if (dataCount == 255)
        dataCount += AacGetBits(bsi, 8);

    if (byteAlign)
        ByteAlignBitstream(bsi);

    psi->dataCount = dataCount;
    dataBuf = psi->dataBuf;
    while (dataCount--)
        *dataBuf++ = AacGetBits(bsi, 8);

    return 0;
}

/**************************************************************************************
 * Function:    DecodeProgramConfigElement
 *
 * Description: decode one PCE
 *
 * Inputs:      BitStreamInfo struct pointing to start of PCE (14496-3, table 4.4.2)
 *
 * Outputs:     filled-in ProgConfigElement struct
 *              updated BitStreamInfo struct
 *
 * Return:      0 if successful, error code (< 0) if error
 *
 * Notes:       #define KEEP_PCE_COMMENTS to save the comment field of the PCE
 *                (otherwise we just skip it in the bitstream, to save memory)
 **************************************************************************************/
int DecodeProgramConfigElement(ProgConfigElement *pce, BitStreamInfo *bsi)
{
    int i;

    pce->elemInstTag =   AacGetBits(bsi, 4);
    pce->profile =       AacGetBits(bsi, 2);
    pce->sampRateIdx =   AacGetBits(bsi, 4);
    pce->numFCE =        AacGetBits(bsi, 4);
    pce->numSCE =        AacGetBits(bsi, 4);
    pce->numBCE =        AacGetBits(bsi, 4);
    pce->numLCE =        AacGetBits(bsi, 2);
    pce->numADE =        AacGetBits(bsi, 3);
    pce->numCCE =        AacGetBits(bsi, 4);

    pce->monoMixdown = AacGetBits(bsi, 1) << 4; /* present flag */
    if (pce->monoMixdown)
        pce->monoMixdown |= AacGetBits(bsi, 4); /* element number */

    pce->stereoMixdown = AacGetBits(bsi, 1) << 4;   /* present flag */
    if (pce->stereoMixdown)
        pce->stereoMixdown  |= AacGetBits(bsi, 4);  /* element number */

    pce->matrixMixdown = AacGetBits(bsi, 1) << 4;   /* present flag */
    if (pce->matrixMixdown)
    {
        pce->matrixMixdown  |= AacGetBits(bsi, 2) << 1; /* index */
        pce->matrixMixdown  |= AacGetBits(bsi, 1);          /* pseudo-surround enable */
    }

    for (i = 0; i < pce->numFCE; i++)
    {
        pce->fce[i]  = AacGetBits(bsi, 1) << 4; /* is_cpe flag */
        pce->fce[i] |= AacGetBits(bsi, 4);          /* tag select */
    }

    for (i = 0; i < pce->numSCE; i++)
    {
        pce->sce[i]  = AacGetBits(bsi, 1) << 4; /* is_cpe flag */
        pce->sce[i] |= AacGetBits(bsi, 4);          /* tag select */
    }

    for (i = 0; i < pce->numBCE; i++)
    {
        pce->bce[i]  = AacGetBits(bsi, 1) << 4; /* is_cpe flag */
        pce->bce[i] |= AacGetBits(bsi, 4);          /* tag select */
    }

    for (i = 0; i < pce->numLCE; i++)
        pce->lce[i] = AacGetBits(bsi, 4);           /* tag select */

    for (i = 0; i < pce->numADE; i++)
        pce->ade[i] = AacGetBits(bsi, 4);           /* tag select */

    for (i = 0; i < pce->numCCE; i++)
    {
        pce->cce[i]  = AacGetBits(bsi, 1) << 4; /* independent/dependent flag */
        pce->cce[i] |= AacGetBits(bsi, 4);          /* tag select */
    }


    ByteAlignBitstream(bsi);

#ifdef KEEP_PCE_COMMENTS
    pce->commentBytes = AacGetBits(bsi, 8);
    for (i = 0; i < pce->commentBytes; i++)
        pce->commentField[i] = AacGetBits(bsi, 8);
#else
    /* eat comment bytes and throw away */
    i = AacGetBits(bsi, 8);
    while (i--)
        AacGetBits(bsi, 8);
#endif

    return 0;
}

/**************************************************************************************
 * Function:    DecodeFillElement
 *
 * Description: decode one fill element
 *
 * Inputs:      BitStreamInfo struct pointing to start of fill element
 *                (14496-3, table 4.4.11)
 *
 * Outputs:     updated element instance tag
 *              unpacked extension payload
 *
 * Return:      0 if successful, -1 if error
 **************************************************************************************/
static int DecodeFillElement(AACDecInfo *aacDecInfo, BitStreamInfo *bsi)
{
    unsigned int fillCount;
    unsigned char *fillBuf;
    PSInfoBase *psi;

    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return -1;
    psi = (PSInfoBase *)(aacDecInfo->psInfoBase);

    fillCount = AacGetBits(bsi, 4);
    if (fillCount == 15)
        fillCount += (AacGetBits(bsi, 8) - 1);

    psi->fillCount = fillCount;
    fillBuf = psi->fillBuf;
    while (fillCount--)
        *fillBuf++ = AacGetBits(bsi, 8);

    aacDecInfo->currInstTag = -1;   /* fill elements don't have instance tag */
    aacDecInfo->fillExtType = 0;

#ifdef AAC_ENABLE_SBR
    /* check for SBR
     * aacDecInfo->sbrEnabled is sticky (reset each raw_data_block), so for multichannel
     *    need to verify that all SCE/CPE/ICCE have valid SBR fill element following, and
     *    must upsample by 2 for LFE
     */
    if (psi->fillCount > 0)
    {
        aacDecInfo->fillExtType = (int)((psi->fillBuf[0] >> 4) & 0x0f);
        if (aacDecInfo->fillExtType == EXT_SBR_DATA || aacDecInfo->fillExtType == EXT_SBR_DATA_CRC)
            aacDecInfo->sbrEnabled = 1;
    }
#endif

    aacDecInfo->fillBuf = psi->fillBuf;
    aacDecInfo->fillCount = psi->fillCount;

    return 0;
}

/**************************************************************************************
 * Function:    DecodeNextElement
 *
 * Description: decode next syntactic element in AAC frame
 *
 * Inputs:      valid AACDecInfo struct
 *              double pointer to buffer containing next element
 *              pointer to bit offset
 *              pointer to number of valid bits remaining in buf
 *
 * Outputs:     type of element decoded (aacDecInfo->currBlockID)
 *              type of element decoded last time (aacDecInfo->prevBlockID)
 *              updated aacDecInfo state, depending on which element was decoded
 *              updated buffer pointer
 *              updated bit offset
 *              updated number of available bits
 *
 * Return:      0 if successful, error code (< 0) if error
 **************************************************************************************/
int DecodeNextElement(AACDecInfo *aacDecInfo, unsigned char **buf, int *bitOffset, int *bitsAvail)
{
    int err, bitsUsed;
    PSInfoBase *psi;
    BitStreamInfo bsi;

    /* validate pointers */
    if (!aacDecInfo || !aacDecInfo->psInfoBase)
        return ERR_AAC_NULL_POINTER;
    psi = (PSInfoBase *)(aacDecInfo->psInfoBase);

    /* init bitstream reader */
    SetBitstreamPointer(&bsi, (*bitsAvail + 7) >> 3, *buf);
    AacGetBits(&bsi, *bitOffset);

    /* read element ID (save last ID for SBR purposes) */
    aacDecInfo->prevBlockID = aacDecInfo->currBlockID;
    aacDecInfo->currBlockID = AacGetBits(&bsi, NUM_SYN_ID_BITS);

    /* set defaults (could be overwritten by DecodeXXXElement(), depending on currBlockID) */
    psi->commonWin = 0;

    err = 0;
    switch (aacDecInfo->currBlockID)
    {
        case AAC_ID_SCE:
            err = DecodeSingleChannelElement(aacDecInfo, &bsi);
            break;
        case AAC_ID_CPE:
            err = DecodeChannelPairElement(aacDecInfo, &bsi);
            break;
        case AAC_ID_CCE:
            /* TODO - implement CCE decoding */
            break;
        case AAC_ID_LFE:
            err = DecodeLFEChannelElement(aacDecInfo, &bsi);
            break;
        case AAC_ID_DSE:
            err = DecodeDataStreamElement(aacDecInfo, &bsi);
            break;
        case AAC_ID_PCE:
            err = DecodeProgramConfigElement(psi->pce + 0, &bsi);
            break;
        case AAC_ID_FIL:
            err = DecodeFillElement(aacDecInfo, &bsi);
            break;
        case AAC_ID_END:
            break;
    }
    if (err)
        return ERR_AAC_SYNTAX_ELEMENT;

    /* update bitstream reader */
    bitsUsed = CalcBitsUsed(&bsi, *buf, *bitOffset);
    *buf += (bitsUsed + *bitOffset) >> 3;
    *bitOffset = (bitsUsed + *bitOffset) & 0x07;
    *bitsAvail -= bitsUsed;

    if (*bitsAvail < 0)
        return ERR_AAC_INDATA_UNDERFLOW;

    return ERR_AAC_NONE;
}

