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
 * Function:    Subband
 *
 * Description: do subband transform on all the blocks in one granule, all channels
 *
 * Inputs:      filled MP3DecInfo structure, after calling IMDCT for all channels
 *              vbuf[ch] and vindex[ch] must be preserved between calls
 *
 * Outputs:     decoded PCM data, interleaved LRLRLR... if stereo
 *
 * Return:      0 on success,  -1 if null input pointers
 **************************************************************************************/
int Subband(MP3DecInfo *mp3DecInfo, short *pcmBuf)
{
    int b;
    HuffmanInfo *hi;
    IMDCTInfo *mi;
    SubbandInfo *sbi;

    /* validate pointers */
    if (!mp3DecInfo || !mp3DecInfo->HuffmanInfoPS || !mp3DecInfo->IMDCTInfoPS || !mp3DecInfo->SubbandInfoPS)
        return -1;

    hi = (HuffmanInfo *)mp3DecInfo->HuffmanInfoPS;
    mi = (IMDCTInfo *)(mp3DecInfo->IMDCTInfoPS);
    sbi = (SubbandInfo*)(mp3DecInfo->SubbandInfoPS);

    if (mp3DecInfo->nChans == 2)
    {
        /* stereo */
        for (b = 0; b < BLOCK_SIZE; b++)
        {
            FDCT32(mi->outBuf[0][b], sbi->vbuf + 0*32, sbi->vindex, (b & 0x01), mi->gb[0]);
            FDCT32(mi->outBuf[1][b], sbi->vbuf + 1*32, sbi->vindex, (b & 0x01), mi->gb[1]);
            PolyphaseStereo(pcmBuf, sbi->vbuf + sbi->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
            sbi->vindex = (sbi->vindex - (b & 0x01)) & 7;
            pcmBuf += (2 * NBANDS);
        }
    }
    else
    {
        /* mono */
        for (b = 0; b < BLOCK_SIZE; b++)
        {
            FDCT32(mi->outBuf[0][b], sbi->vbuf + 0*32, sbi->vindex, (b & 0x01), mi->gb[0]);
            PolyphaseMono(pcmBuf, sbi->vbuf + sbi->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
            sbi->vindex = (sbi->vindex - (b & 0x01)) & 7;
            pcmBuf += NBANDS;
        }
    }

    return 0;
}

