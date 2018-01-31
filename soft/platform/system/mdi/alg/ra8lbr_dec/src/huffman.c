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

/**************************************************************************************
 * Function:    DecodeHuffmanScalar
 *
 * Description: decode one Huffman symbol from bitstream
 *
 * Inputs:      pointers to table and info
 *              right-aligned bit buffer with MAX_HUFF_BITS bits
 *              pointer to receive decoded symbol
 *
 * Outputs:     decoded symbol
 *
 * Return:      number of bits in symbol
 **************************************************************************************/
int DecodeHuffmanScalar(const unsigned short *huffTab, const HuffInfo *huffTabInfo, int bitBuf, int *val)
{
    const unsigned char *countPtr;
    unsigned int cache, total, count, t;
    const unsigned short *map;

    map = huffTab + huffTabInfo->offset;
    countPtr = huffTabInfo->count;
    cache = (unsigned int)(bitBuf << (17 - MAX_HUFF_BITS));

    total = 0;
    count = *countPtr++;
    t = count << 16;

    while (cache >= t)
    {
        cache -=  t;
        cache <<= 1;
        total += count;
        count = *countPtr++;
        t = count << 16;
    }
    *val = map[total + (cache >> 16)];

    /* return number of bits in symbol */
    return (countPtr - huffTabInfo->count);
}
