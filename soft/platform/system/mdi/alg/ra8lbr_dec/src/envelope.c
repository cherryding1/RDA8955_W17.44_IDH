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





#include "coder.h"

/* coding params for rms[0] */
#define RMS0BITS     6  /* bits for env[0] */
#define RMS0MIN     -6  /* arbitrary! */
#define CODE2RMS(i) ((i)+(RMS0MIN))

/**************************************************************************************
 * Function:    DecodeEnvelope
 *
 * Description: decode the power envelope
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              number of bits remaining in bitstream for this frame
 *              index of current channel
 *
 * Outputs:     rmsIndex[0, ..., cregsions-1] has power index for each region
 *              updated rmsMax with largest value of rmsImdex for this channel
 *
 * Return:      number of bits remaining in bitstream, -1 if out-of-bits
 **************************************************************************************/
int DecodeEnvelope(Gecko2Info *gi, int availbits, int ch)
{
    int r, code, nbits, rprime, cache, rmsMax;
    int *rmsIndex = gi->db.rmsIndex;
    BitStreamInfo *bsi = &(gi->bsi);

    if (availbits < RMS0BITS)
        return -1;

    /* unpack first index */
    code = RA_GETBITS(bsi, RMS0BITS);
    availbits -= RMS0BITS;
    rmsIndex[0] = CODE2RMS(code);

    /* check for escape code */
    /* ASSERT(rmsIndex[0] != 0); */

    rmsMax = rmsIndex[0];
    for (r = 1; r < gi->cRegions; r++)
    {

        /* for interleaved regions, choose a reasonable table */
        if (r < 2 * gi->cplStart)
        {
            rprime = r >> 1;
            if (rprime < 1)
                rprime = 1;
        }
        else
        {
            rprime = r - gi->cplStart;
        }

        /* above NUM_POWTABLES, always use the same Huffman table */
        if (rprime > NUM_POWTABLES)
            rprime = NUM_POWTABLES;

        cache = RA_TRYBITS(bsi, MAX_HUFF_BITS);
        nbits = DecodeHuffmanScalar(huffTabPower, &huffTabPowerInfo[rprime-1], cache, &code);

        availbits -= nbits;
        RA_SKIPBITS(bsi, nbits);

        /* encoder uses differential coding with differences constrained to the range [-12, 11] */
        rmsIndex[r] = rmsIndex[r-1] + (code - 12);
        if (rmsIndex[r] > rmsMax)
            rmsMax = rmsIndex[r];
    }
    gi->rmsMax[ch] = rmsMax;

    return availbits;
}
