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




#include "mcip_debug.h"
#include "hal_timers.h"

#include "coder.h"

/**************************************************************************************
 * Function:    Gecko2InitDecoder
 *
 * Description: initialize the fixed-point Gecko2 audio decoder
 *
 * Inputs:      number of samples per frame
 *              number of channels
 *              number of frequency regions coded
 *              number of encoded bits per frame
 *              number of samples per second
 *              start region for coupling (joint stereo only)
 *              number of bits for each coupling scalefactor (joint stereo only)
 *              pointer to receive number of frames of coding delay
 *
 * Outputs:     number of frames of coding delay (i.e. discard the PCM output from
 *                the first *codingDelay calls to Gecko2Decode())
 *
 * Return:      instance pointer, 0 if error (malloc fails, unsupported mode, etc.)
 *
 * Notes:       this implementation is fully reentrant and thread-safe - the
 *                HGecko2Decoder instance pointer tracks all the state variables
 *                for each instance
 **************************************************************************************/
HGecko2Decoder Gecko2InitDecoder(int nSamples, int nChannels, int nRegions, int nFrameBits, int sampRate,
                                 int cplStart, int cplQbits, int *codingDelay)
{
    Gecko2Info *gi;

    /* check parameters */
    if (nChannels < 0 || nChannels > 2)
        return 0;
    if (nRegions < 0 || nRegions > MAXREGNS)
        return 0;
    if (nFrameBits < 0 || cplStart < 0)
        return 0;
    if (cplQbits && (cplQbits < 2 || cplQbits > 6))
        return 0;

    gi = AllocateBuffers();
    if (!gi)
        return 0;

    /* if stereo, cplQbits == 0 means dual-mono, > 0 means joint stereo */
    gi->jointStereo = (nChannels == 2) && (cplQbits > 0);

    gi->nSamples = nSamples;
    gi->nChannels = nChannels;
    gi->nRegions = nRegions;
    gi->nFrameBits = nFrameBits;
    if (gi->nChannels == 2 && !gi->jointStereo)
        gi->nFrameBits /= 2;
    gi->sampRate = sampRate;

    if (gi->jointStereo)
    {
        /* joint stereo */
        gi->cplStart = cplStart;
        gi->cplQbits = cplQbits;
        gi->rateBits = 5;
        if (gi->nSamples > 256)
            gi->rateBits++;
        if (gi->nSamples > 512)
            gi->rateBits++;
    }
    else
    {
        /* mono or dual-mono */
        gi->cplStart = 0;
        gi->cplQbits = 0;
        gi->rateBits = 5;
    }
    gi->cRegions = gi->nRegions + gi->cplStart;
    gi->nCatzns = (1 << gi->rateBits);
    gi->lfsr[0] = gi->lfsr[1] = ('k' | 'e' << 8 | 'n' << 16 | 'c' << 24);       /* well-chosen seed for dither generator */

    /* validate tranform size */
    if (gi->nSamples == 256)
    {
        gi->xformIdx = 0;
    }
    else if (gi->nSamples == 512)
    {
        gi->xformIdx = 1;
    }
    else if (gi->nSamples == 1024)
    {
        gi->xformIdx = 2;
    }
    else
    {
        Gecko2FreeDecoder(gi);
        return 0;
    }

    /* this is now 2, since lookahead MLT has been removed */
    *codingDelay = CODINGDELAY;

    return (HGecko2Decoder)gi;
}

/**************************************************************************************
 * Function:    Gecko2FreeDecoder
 *
 * Description: free the fixed-point Gecko2 audio decoder
 *
 * Inputs:      HGecko2Decoder instance pointer returned by Gecko2InitDecoder()
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void Gecko2FreeDecoder(HGecko2Decoder hGecko2Decoder)
{
    Gecko2Info *gi = (Gecko2Info *)hGecko2Decoder;
    if (!gi)
        return;

    FreeBuffers(gi);

    return;
}

/**************************************************************************************
 * Function:    Gecko2ClearBadFrame
 *
 * Description: zero out pcm buffer if error decoding Gecko2 frame
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              pointer to pcm output buffer
 *
 * Outputs:     zeroed out pcm buffer
 *              zeroed out data buffers (as if codec had been reinitialized)
 *
 * Return:      none
 **************************************************************************************/
static void Gecko2ClearBadFrame(Gecko2Info *gi, short *outbuf)
{
    int i, ch;

    if (!gi || gi->nSamples * gi->nChannels > MAXNSAMP * MAXNCHAN || gi->nSamples * gi->nChannels < 0)
        return;

    /* clear PCM buffer */
    for (i = 0; i < gi->nSamples * gi->nChannels; i++)
        outbuf[i] = 0;

    /* clear internal data buffers */
    for (ch = 0; ch < gi->nChannels; ch++)
    {
        for (i = 0; i < gi->nSamples; i++)
        {
            gi->db.decmlt[ch][i] = 0;
            gi->db.decmlt[ch][i + MAXNSAMP] = 0;
        }
        gi->xbits[ch][0] = gi->xbits[ch][1] = 0;
    }

}

/**************************************************************************************
 * Function:    Gecko2Decode
 *
 * Description: decode one frame of audio data
 *
 * Inputs:      HGecko2Decoder instance pointer returned by Gecko2InitDecoder()
 *              pointer to one encoded frame
 *                (nFrameBits / 8 bytes of data, byte-aligned)
 *              flag indicating lost frame (lostflag != 0 means lost)
 *              pointer to receive one decoded frame of PCM
 *
 * Outputs:     one frame (nSamples * nChannels 16-bit samples) of decoded PCM
 *
 * Return:      0 if frame decoded okay, error code (< 0) if error
 *
 * Notes:       to reduce memory and CPU usage, this only implements one-sided
 *                (backwards) interpolation for error concealment (no lookahead)
 **************************************************************************************/
int Gecko2Decode(HGecko2Decoder hGecko2Decoder, unsigned char *codebuf, int lostflag, short *outbuf)
{
    int i, ch, availbits, gbMin[MAXNCHAN];
    Gecko2Info *gi = (Gecko2Info *)hGecko2Decoder;

    //int32 time0,time1;

    if (!gi)
        return -1;

    if (lostflag)
    {
        ch = 0;
        for (i = 0; i < gi->nSamples; i++)
        {
            gi->db.decmlt[ch][i] = (gi->db.decmlt[ch][i] >> 1) + (gi->db.decmlt[ch][i] >> 2);
        }

        //time0=hal_TimGetUpTime();

    }
    else
    {
        /* current frame is valid, decode it */
        if (gi->jointStereo)
        {
            /* decode gain control info, coupling coefficients, and power envlope */
            availbits = DecodeSideInfo(gi, codebuf, gi->nFrameBits, 0);
            if (availbits < 0)
            {
                Gecko2ClearBadFrame(gi, outbuf);
                return ERR_GECKO2_INVALID_SIDEINFO;
            }

            /* reconstruct power envelope */
            CategorizeAndExpand(gi, availbits);

            /* reconstruct full MLT, including stereo decoupling */
            gbMin[0] = gbMin[1] = DecodeTransform(gi, gi->db.decmlt[0], availbits, &gi->lfsr[0], 0);
            JointDecodeMLT(gi, gi->db.decmlt[0], gi->db.decmlt[1]);
            gi->xbits[1][1] = gi->xbits[0][1];
        }
        else
        {
            ch = 0;
            /* decode gain control info and power envlope */
            availbits = DecodeSideInfo(gi, codebuf + (ch * gi->nFrameBits >> 3), gi->nFrameBits, ch);
            if (availbits < 0)
            {
                Gecko2ClearBadFrame(gi, outbuf);
                return ERR_GECKO2_INVALID_SIDEINFO;
            }

            /* reconstruct power envelope */
            CategorizeAndExpand(gi, availbits);

            /* reconstruct full MLT */
            gbMin[ch] = DecodeTransform(gi, gi->db.decmlt[ch], availbits, &gi->lfsr[ch], ch);

            /* zero out non-coded regions */
            for (i = gi->nRegions*NBINS; i < gi->nSamples; i++)
                gi->db.decmlt[ch][i] = 0;
        }

        //time0=hal_TimGetUpTime();

        /* inverse transform, without window or overlap-add */
        IMLTNoWindow(gi->xformIdx, gi->db.decmlt[0], gbMin[0]);

    }

    /* apply synthesis window, gain window, then overlap-add (interleaves stereo PCM LRLR...) */
    if (gi->dgainc[0][0].nats || gi->dgainc[0][1].nats || gi->xbits[0][0] || gi->xbits[0][1])
        DecWindowWithAttacks(gi->xformIdx, gi->db.decmlt[0], outbuf, gi->nChannels, &gi->dgainc[0][0], &gi->dgainc[0][1], gi->xbits[0]);
    else
        DecWindowNoAttacks(gi->xformIdx, gi->db.decmlt[0], outbuf, gi->nChannels);

    for (ch = 0; ch < gi->nChannels; ch++)
    {
        CopyGainInfo(&gi->dgainc[ch][0], &gi->dgainc[ch][1]);
        gi->xbits[ch][0] = gi->xbits[ch][1];
    }

    //time1=hal_TimGetUpTime();
    //diag_printf("ra rec=%d",time1-time0);

    return 0;
}


#ifdef SHEEN_VC_DEBUG
//test
#include <stdio.h>

void main(void)
{
    FILE *pIn, *pOut;
    char *pBufIn, *pBufOut;
    int res,i;

    HGecko2Decoder pHdl=0;

#if 0
    int nChannels=  1;//2;
    int nFrameBits= 32*8;//280;
    int sampRate=   11025;//44100;

    int nSamples=   256/nChannels;//2048;
    int nRegions=   12;//37;
    int cplStart=   0;//8;
    int cplQbits=   0;//5;
    int codingDelay=0;
#else
    int nChannels=  2;
    int nFrameBits= 280*8;
    int sampRate=   44100;

    int nSamples=   2048/nChannels;
    int nRegions=   37;
    int cplStart=   8;
    int cplQbits=   5;
    int codingDelay=0;
#endif

    int lostflag=0;

    pIn=fopen("D:\\测试文件\\test.ra","rb");
    pOut=fopen("D:\\测试文件\\cook.pcm","wb");

    pBufIn=malloc(2048);
    pBufOut=malloc(8*1024);

    pHdl=Gecko2InitDecoder(nSamples, nChannels, nRegions, nFrameBits, sampRate,
                           cplStart, cplQbits, &codingDelay);

    i=0;

    while (1)
    {
        res=fread(pBufIn,1,nFrameBits/8,pIn);
        if(res <=0)break;

        res=Gecko2Decode(pHdl, pBufIn, lostflag, pBufOut);

        fwrite(pBufOut,1,nSamples*nChannels*2,pOut);

        printf("frame %d res=%d \n",i,res);
        i++;
        lostflag=0;
    }


    Gecko2FreeDecoder(pHdl);

    fclose(pIn);
    fclose(pOut);

    free(pBufIn);
    free(pBufOut);

    return;
}

#endif