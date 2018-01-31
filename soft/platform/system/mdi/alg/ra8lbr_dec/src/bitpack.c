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
#include "assembly.h"

const unsigned char pkkey[8] =  { 0x37, 0xc5, 0x11, 0xf2,0x37, 0xc5, 0x11, 0xf2 };

/**************************************************************************************
 * Function:    GetBits
 *
 * Description: get bits from bitstream, optionally advance bitstream pointer
 *
 * Inputs:      pointer to initialized BitStreamInfo struct
 *              number of bits to get from bitstream
 *              flag to indicate whether to advance bitstream pointer or not
 *
 * Outputs:     updated bitstream info struct (if advanceFlag set)
 *
 * Return:      the next nBits bits of data from bitstream buffer (right-justified)
 *
 * Notes:       nBits must be in range [0, 31], nBits outside this range masked by 0x1f
 *              for speed, does not indicate error if you overrun bit buffer
 *              if nBits = 0, returns 0
 *              applies XOR key when reading (rather than using out-of-place buffer)
 *              reads byte-at-a-time (not cached 32-bit ints) so not designed to be
 *                especially fast (i.e. handy for Huffman decoding of a few symbols,
 *                but not optimal for decoding long sequences of codewords)
 **************************************************************************************/
/*
unsigned int GetBits(BitStreamInfo *bsi, int nBits, int advanceFlag)
{
    int readBits, off, key, nBytes;
    unsigned int data;
    unsigned char *buf;

    nBits &= 0x1f;
    if (!nBits)
        return 0;

    buf = bsi->buf;
    off = bsi->off;
    key = bsi->key;

    data = ((unsigned int)(*buf++ ^ pkkey[key++])) << (24 + off);
    key &= 0x03;
    readBits = 8 - off;

    while (readBits < nBits && readBits <= 24) {
        data |= ((unsigned int)(*buf++ ^ pkkey[key++])) << (24 - readBits);
        key &= 0x03;
        readBits += 8;
    }

    if (readBits < nBits)
        data |= ((unsigned int)(*buf++ ^ pkkey[key])) >> (readBits - 24);

    if (advanceFlag) {
        nBytes = (bsi->off + nBits) >> 3;
        bsi->buf += nBytes;
        bsi->off = (bsi->off + nBits) & 0x07;
        bsi->key = (bsi->key + nBytes) & 0x03;
    }

    return (data >> (32 - nBits));
}
*/

unsigned int RA_GETBITS(BitStreamInfo *bsi, unsigned int len)
{
    unsigned int bits;
    bits = RA_TRYBITS(bsi,len);
    RA_SKIPBITS(bsi,len);
    return bits;
}

/*
void AdvanceBitstream(BitStreamInfo *bsi, int nBits)
{
    int nBytes;

    nBytes = (bsi->off + nBits) >> 3;
    bsi->buf += nBytes;
    bsi->off = (bsi->off + nBits) & 0x07;
    bsi->key = (bsi->key + nBytes) & 0x03;
}
*/

/**************************************************************************************
 * Function:    DecodeSideInfo
 *
 * Description: parse bitstream and decode gain info, coupling info, power envelope,
 *                and categorization code
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              pointer to bitstream buffer (byte-aligned)
 *              number of bits available in buf
 *              channel index
 *
 * Outputs:     filled-in dgainc structs, channel coupling indices (if joint stereo),
 *                power envelope index for each region,
 *                and rate code (selected categorization)
 *
 * Return:      number of bits remaining in bitstream, -1 if out-of-bits
 *
 * Notes:       encoder guarantees that gain, couple, envelope, and rateCode do
 *                not run out of bits
 **************************************************************************************/
int DecodeSideInfo(Gecko2Info *gi, unsigned char *buf, int availbits, int ch)
{
    BitStreamInfo *bsi = &(gi->bsi);

    /* init bitstream reader */
    bsi->buf = buf;
    bsi->off = 0;
    bsi->key = 0;

    bsi->bitbuf =  (unsigned int)(bsi->buf[0]^ pkkey[bsi->key])  << 24;
    bsi->bitbuf |= (unsigned int)(bsi->buf[1]^ pkkey[bsi->key+1])<< 16;
    bsi->bitbuf |= (unsigned int)(bsi->buf[2]^ pkkey[bsi->key+2])<< 8;
    bsi->bitbuf |= (unsigned int)(bsi->buf[3]^ pkkey[bsi->key+3]);

    /* decode gain info */
    availbits = DecodeGainInfo(gi, &(gi->dgainc[ch][1]), availbits);

    /* replicate gain control and decode coupling info, if joint stereo */
    if (gi->jointStereo)
    {
        ASSERT(ch == 0);
        CopyGainInfo(&gi->dgainc[1][1], &gi->dgainc[0][1]);

        availbits = DecodeCoupleInfo(gi, availbits);
    }

    /* decode power envelope (return error if runs out of bits) */
    availbits = DecodeEnvelope(gi, availbits, ch);

    /* decode rate code (return error if runs out of bits) */
    if (availbits < gi->rateBits)
        return -1;

    gi->rateCode = RA_GETBITS(bsi, gi->rateBits);
    availbits -= gi->rateBits;

    return availbits;
}
