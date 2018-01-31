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





#include "mp3_coder.h"
#include "mp3_assembly.h"

/**************************************************************************************
 * Function:    Dequantize
 *
 * Description: dequantize coefficients, decode stereo, reorder short blocks
 *                (one granule-worth)
 *
 * Inputs:      MP3DecInfo structure filled by UnpackFrameHeader(), UnpackSideInfo(),
 *                UnpackScaleFactors(), and DecodeHuffman() (for this granule)
 *              index of current granule
 *
 * Outputs:     dequantized and reordered coefficients in hi->huffDecBuf
 *                (one granule-worth, all channels), format = Q26
 *              operates in-place on huffDecBuf but also needs di->workBuf
 *              updated hi->nonZeroBound index for both channels
 *
 * Return:      0 on success, -1 if null input pointers
 *
 * Notes:       In calling output Q(DQ_FRACBITS_OUT), we assume an implicit bias
 *                of 2^15. Some (floating-point) reference implementations factor this
 *                into the 2^(0.25 * gain) scaling explicitly. But to avoid precision
 *                loss, we don't do that. Instead take it into account in the final
 *                round to PCM (>> by 15 less than we otherwise would have).
 *              Equivalently, we can think of the dequantized coefficients as
 *                Q(DQ_FRACBITS_OUT - 15) with no implicit bias.
 **************************************************************************************/
int Dequantize(MP3DecInfo *mp3DecInfo, int gr)
{
    int i, ch, nSamps, mOut[2];
    FrameHeader *fh;
    SideInfo *si;
    ScaleFactorInfo *sfi;
    HuffmanInfo *hi;
    DequantInfo *di;
    CriticalBandInfo *cbi;

    /* validate pointers */
    if (!mp3DecInfo || !mp3DecInfo->FrameHeaderPS || !mp3DecInfo->SideInfoPS || !mp3DecInfo->ScaleFactorInfoPS ||
            !mp3DecInfo->HuffmanInfoPS || !mp3DecInfo->DequantInfoPS)
        return -1;

    fh = (FrameHeader *)(mp3DecInfo->FrameHeaderPS);

    /* si is an array of up to 4 structs, stored as gr0ch0, gr0ch1, gr1ch0, gr1ch1 */
    si = (SideInfo *)(mp3DecInfo->SideInfoPS);
    sfi = (ScaleFactorInfo *)(mp3DecInfo->ScaleFactorInfoPS);
    hi = (HuffmanInfo *)mp3DecInfo->HuffmanInfoPS;
    di = (DequantInfo *)mp3DecInfo->DequantInfoPS;
    cbi = di->cbi;
    mOut[0] = mOut[1] = 0;

    /* dequantize all the samples in each channel */
    for (ch = 0; ch < mp3DecInfo->nChans; ch++)
    {
        hi->gb[ch] = DequantChannel(hi->huffDecBuf[ch], di->workBuf, &hi->nonZeroBound[ch], fh,
                                    &si->sis[gr][ch], &sfi->sfis[gr][ch], &cbi[ch]);
    }

    /* joint stereo processing assumes one guard bit in input samples
     * it's extremely rare not to have at least one gb, so if this is the case
     *   just make a pass over the data and clip to [-2^30+1, 2^30-1]
     * in practice this may never happen
     */
    if (fh->modeExt && (hi->gb[0] < 1 || hi->gb[1] < 1))
    {
        for (i = 0; i < hi->nonZeroBound[0]; i++)
        {
            if (hi->huffDecBuf[0][i] < -0x3fffffff)  hi->huffDecBuf[0][i] = -0x3fffffff;
            if (hi->huffDecBuf[0][i] >  0x3fffffff)  hi->huffDecBuf[0][i] =  0x3fffffff;
        }
        for (i = 0; i < hi->nonZeroBound[1]; i++)
        {
            if (hi->huffDecBuf[1][i] < -0x3fffffff)  hi->huffDecBuf[1][i] = -0x3fffffff;
            if (hi->huffDecBuf[1][i] >  0x3fffffff)  hi->huffDecBuf[1][i] =  0x3fffffff;
        }
    }

    /* do mid-side stereo processing, if enabled */
    if (fh->modeExt >> 1)
    {
        if (fh->modeExt & 0x01)
        {
            /* intensity stereo enabled - run mid-side up to start of right zero region */
            if (cbi[1].cbType == 0)
                nSamps = fh->sfBand->l[cbi[1].cbEndL + 1];
            else
                nSamps = 3 * fh->sfBand->s[cbi[1].cbEndSMax + 1];
        }
        else
        {
            /* intensity stereo disabled - run mid-side on whole spectrum */
            nSamps = MAX(hi->nonZeroBound[0], hi->nonZeroBound[1]);
        }
        MidSideProc(hi->huffDecBuf, nSamps, mOut);
    }

    /* do intensity stereo processing, if enabled */
    if (fh->modeExt & 0x01)
    {
        nSamps = hi->nonZeroBound[0];
        if (fh->ver == MPEG1)
        {
            IntensityProcMPEG1(hi->huffDecBuf, nSamps, fh, &sfi->sfis[gr][1], di->cbi,
                               fh->modeExt >> 1, si->sis[gr][1].mixedBlock, mOut);
        }
        else
        {
            IntensityProcMPEG2(hi->huffDecBuf, nSamps, fh, &sfi->sfis[gr][1], di->cbi, &sfi->sfjs,
                               fh->modeExt >> 1, si->sis[gr][1].mixedBlock, mOut);
        }
    }

    /* adjust guard bit count and nonZeroBound if we did any stereo processing */
    if (fh->modeExt)
    {
        hi->gb[0] = CLZ(mOut[0]) - 1;
        hi->gb[1] = CLZ(mOut[1]) - 1;
        nSamps = MAX(hi->nonZeroBound[0], hi->nonZeroBound[1]);
        hi->nonZeroBound[0] = nSamps;
        hi->nonZeroBound[1] = nSamps;
    }

    /* output format Q(DQ_FRACBITS_OUT) */
    return 0;
}
