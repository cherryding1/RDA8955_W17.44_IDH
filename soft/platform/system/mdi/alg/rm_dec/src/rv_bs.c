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



























#include "string.h"
#include "beginhdr.h"
#include "rvdebug.h"
#include "bsx.h"


/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::GetBsSize() */
/*      Returns the size of the bitstream data in bytes based on the */
/*      current position of the buffer pointer, m_pbs. */
/* --------------------------------------------------------------------------- */

U32 CB_GetBsSize(struct CRealVideoBs *t)
{
    U32 size;

    RVAssert(t->m_pbs >= t->m_pbsBase);
    size = (U32)(t->m_pbs - t->m_pbsBase) + 1;
    return(!t->m_bitOffset ? (size - 1) : size);

} /* CBaseBitstream::GetBsSize() */


/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::GetBsOffset() */
/*      Returns the bit position of the buffer pointer relative to the  */
/*      beginning of the buffer. */
/* --------------------------------------------------------------------------- */

U32 CB_GetBsOffset(struct CRealVideoBs *t)
{
    RVAssert(t->m_pbs >= t->m_pbsBase);
    return((U32)(t->m_pbs - t->m_pbsBase) * 8 + t->m_bitOffset);

} /* CBaseBitstream::GetBsOffset() */


/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::GetBsBase(CRealVideoBs *t) */
/*      Returns the base pointer to the beginning of the bitstream. */
/* --------------------------------------------------------------------------- */

PU8 CB_GetBsBase(struct CRealVideoBs *t)
{
    return(t->m_pbsBase);

} /* CBaseBitstream::GetBsBase() */


/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::GetBsMaxSize() */
/*      Returns the maximum bitstream size. */
/* --------------------------------------------------------------------------- */

U32 CB_GetMaxBsSize(struct CRealVideoBs *t)
{
    return(t->m_maxBsSize);

} /* CBaseBitstream::GetBsMaxSize() */


/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::CheckBsLimit() */
/*      Checks if read/write passed the maximum buffer size. */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_CheckBsLimit(struct CRealVideoBs *t)
{
    U32 size;

    size = CB_GetBsSize(t);

    if (size > t->m_maxBsSize)
    {
        RVDebug((RV_TL_ALWAYS, "ERROR CheckBsLimit: Overshoot buffer, "
                 "%d > %d", size, t->m_maxBsSize));
        return(FALSE);
    }
    return(TRUE);

} /* CBaseBitstream::CheckBsLimit() */



/* --------------------------------------------------------------------------- */
/*  [ENC] CBaseBitstream::UpdateState() */
/*      Advances buffer pointer with given number of bits.  This is only */
/*      used to maintain the bitstream state when calling an assembly  */
/*      routine that accesses the bitstream. */
/*      nbits   : number of bits to advance */
/* --------------------------------------------------------------------------- */

void CB_UpdateState(
    struct CRealVideoBs *t,
    const U32   nbits)
{
    t->m_pbs       += (nbits + t->m_bitOffset) >> 3;
    t->m_bitOffset = (nbits + t->m_bitOffset) & 0x7;

} /* CBaseBitstream::UpdateState() */



/* gs_VLCDecodeTable maps the leading 8-bits of the unread bitstream */
/* into the length and info of the VLC code at the head of those 8 bits. */
/* The table's contents can be summarized as follows, where p, q, r, s */
/* and '*' are arbitrary binary digits {0, 1}. */
/* */
/*  8 Bits      Length  Info */
/*  ========    ======  ==== */
/*  0p0q0r0s    8       pqrs combined with VLC at head of next 8 bits */
/*  0p0q0r1*    7       pqr */
/*  0p0q1***    5       pq */
/*  0p1*****    3       p */
/*  1*******    1       0 */
/* */
/* Note that when the length is 8, or even, we need to read another 8 bits */
/* from the bitstream, look up these 8 bits in the same table, and combine */
/* the length and info with the previously decoded bits. */
/* */
/* For a given 8-bit chunk of bitstream bits, the maximum length is 8, */
/* and the maximum info value is contained in 4 bits.  Thus, we can pack */
/* the length and info in a U8.  The info is in the least significant */
/* 4 bits, and the length is in the most significant 4 bits. */

#define PACK_LENGTH_AND_INFO(length, info) (((length) << 4) | (info))

static const U8 gs_VLCDecodeTable[256] =
{
    PACK_LENGTH_AND_INFO(8,  0),   /* 00000000 */
    PACK_LENGTH_AND_INFO(8,  1),   /* 00000001 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000010 */
    PACK_LENGTH_AND_INFO(7,  0),   /* 00000011 */
    PACK_LENGTH_AND_INFO(8,  2),   /* 00000100 */
    PACK_LENGTH_AND_INFO(8,  3),   /* 00000101 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000110 */
    PACK_LENGTH_AND_INFO(7,  1),   /* 00000111 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001000 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001001 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001010 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001011 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001100 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001101 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001110 */
    PACK_LENGTH_AND_INFO(5,  0),   /* 00001111 */
    PACK_LENGTH_AND_INFO(8,  4),   /* 00010000 */
    PACK_LENGTH_AND_INFO(8,  5),   /* 00010001 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010010 */
    PACK_LENGTH_AND_INFO(7,  2),   /* 00010011 */
    PACK_LENGTH_AND_INFO(8,  6),   /* 00010100 */
    PACK_LENGTH_AND_INFO(8,  7),   /* 00010101 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010110 */
    PACK_LENGTH_AND_INFO(7,  3),   /* 00010111 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011000 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011001 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011010 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011011 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011100 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011101 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011110 */
    PACK_LENGTH_AND_INFO(5,  1),   /* 00011111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00100111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00101111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00110111 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111000 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111001 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111010 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111011 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111100 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111101 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111110 */
    PACK_LENGTH_AND_INFO(3,  0),   /* 00111111 */
    PACK_LENGTH_AND_INFO(8,  8),   /* 01000000 */
    PACK_LENGTH_AND_INFO(8,  9),   /* 01000001 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000010 */
    PACK_LENGTH_AND_INFO(7,  4),   /* 01000011 */
    PACK_LENGTH_AND_INFO(8, 10),   /* 01000100 */
    PACK_LENGTH_AND_INFO(8, 11),   /* 01000101 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000110 */
    PACK_LENGTH_AND_INFO(7,  5),   /* 01000111 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001000 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001001 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001010 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001011 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001100 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001101 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001110 */
    PACK_LENGTH_AND_INFO(5,  2),   /* 01001111 */
    PACK_LENGTH_AND_INFO(8, 12),   /* 01010000 */
    PACK_LENGTH_AND_INFO(8, 13),   /* 01010001 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010010 */
    PACK_LENGTH_AND_INFO(7,  6),   /* 01010011 */
    PACK_LENGTH_AND_INFO(8, 14),   /* 01010100 */
    PACK_LENGTH_AND_INFO(8, 15),   /* 01010101 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010110 */
    PACK_LENGTH_AND_INFO(7,  7),   /* 01010111 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011000 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011001 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011010 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011011 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011100 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011101 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011110 */
    PACK_LENGTH_AND_INFO(5,  3),   /* 01011111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01100111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01101111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01110111 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111000 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111001 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111010 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111011 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111100 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111101 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111110 */
    PACK_LENGTH_AND_INFO(3,  1),   /* 01111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 10111111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11000111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11001111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11010111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11011111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11100111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11101111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110110 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11110111 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111000 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111001 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111010 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111011 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111100 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111101 */
    PACK_LENGTH_AND_INFO(1,  0),   /* 11111110 */
    PACK_LENGTH_AND_INFO(1,  0)    /* 11111111 */
};


/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::GetVLCBits() */
/*        Reads one general VLC code from the bitstream. */
/* --------------------------------------------------------------------------- */

U32
CB_GetVLCBits(struct CRealVideoBs *t, U32 *pInfo)
{

    U32 bits, info = 0, length = 0;
    U32 lengthAndInfo;
    U32 thisChunksLength;

    do
    {
        /* Extract the leading 8-bits into 'bits' */

        if (t->m_bitOffset)
        {
            bits = (U8)((*(t->m_pbs) << t->m_bitOffset) | (t->m_pbs[1] >> (8 - t->m_bitOffset)));
        }
        else
        {
            bits = *(t->m_pbs);
        }

        lengthAndInfo = gs_VLCDecodeTable[bits];
        thisChunksLength = lengthAndInfo >> 4;

        info = (info << (thisChunksLength >> 1)) | (lengthAndInfo & 0xf);
        length += thisChunksLength;

        /* If we consumed all the bits in m_pbs[0], advance m_pbs by 1 */
        t->m_bitOffset += thisChunksLength;
        if (t->m_bitOffset > 7)
        {
            t->m_pbs++;
            t->m_bitOffset -= 8;
        }

    }
    while (!(length & 1));    /* We're done when length is an odd number */

    *pInfo = info;

    return length;
} /* CBaseBitstream::GetVLCBits() */



/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::GetBits() */
/*      Reads bits from buffer.  Supports up to 25 bits. */
/*      nbits   : number of bits to be read */
/* --------------------------------------------------------------------------- */

const U32 GetBitsMask[25] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff
};

U32 CB_GetBits(struct CRealVideoBs *t, const U32 nbits)
{
    U32 n, w;

    /* make sure that the number of bits given is <= 25 */
#ifdef BITSTREAM_ERROR_RESILIENT
    if (nbits > 25)
        return 0;
#else
    RVAssert(nbits <= 25);
#endif

    w = (U32) *(t->m_pbs);

    for (n = t->m_bitOffset + nbits; n >= 8; n -= 8)
    {
        t->m_pbs ++;
        w = (w << 8) | *(t->m_pbs);
    }

    w = (w >> (8 - n)) & GetBitsMask[nbits];
    t->m_bitOffset = n;

    return(w);

} /* CBaseBitstream::GetBits() */



/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::Get1Bit() */
/*      Reads one bit from the buffer. */
/* --------------------------------------------------------------------------- */

U32 CB_Get1Bit(struct CRealVideoBs *t)
{
    U32 w;

    if (t->m_bitOffset == 7)
    {
        w = *(t->m_pbs) & 1;
        t->m_bitOffset = 0;
        t->m_pbs ++;
    }
    else
    {
        w = (*(t->m_pbs) >> (7 - t->m_bitOffset)) & 1;
        t->m_bitOffset ++;
    }

    return(w);

} /* CBaseBitstream::Get1Bit() */


/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::SearchBits() */
/*      Searches for a code with known number of bits.  Bitstream state,  */
/*      pointer and bit offset, will be updated if code found. */
/*      nbits       : number of bits in the code */
/*      code        : code to search for */
/*      lookahead   : maximum number of bits to parse for the code */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_SearchBits(
    struct CRealVideoBs *t,
    const U32   nbits,
    const U32   code,
    const U32   lookahead)
{
    U32 w, n;
    U8* pbs;
    U32 offset;

    pbs     = t->m_pbs;
    offset  = t->m_bitOffset;

    w = CB_GetBits(t,nbits);

    for (n = 0; w != code && n < lookahead; n ++)
        w = ((w << 1) & GetBitsMask[nbits]) | CB_Get1Bit(t);

    if (w == code)
        return(TRUE);
    else
    {
        t->m_pbs        = pbs;
        t->m_bitOffset = offset;
        return(FALSE);
    }

} /* CBaseBitstream::SearchBits() */


/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::SearchBits_IgnoreOnly0() */
/*      Searches for a code with known number of bits.  Skips only zero bits. */
/*      Search fails if code has not been found and non-zero bit encountered. */
/*      nbits       : number of bits in the code */
/*      code        : code to search for */
/*      lookahead   : maximum number of bits to parse for the code */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_SearchBits_IgnoreOnly0(
    struct CRealVideoBs *t,
    const U32   nbits,
    const U32   code,
    const U32   lookahead)
{
    U32 w, n, msb;
    U8* pbs;
    U32 offset;

    pbs     = t->m_pbs;
    offset  = t->m_bitOffset;
    msb     = 1 << (nbits - 1);

    w = CB_GetBits(t,nbits);

    for (n = 0; w != code && n < lookahead; n ++)
    {
        if (w & msb) break;
        w = ((w << 1) & GetBitsMask[nbits]) | CB_Get1Bit(t);
    }

    if (w == code)
        return(TRUE);
    else
    {
        t->m_pbs        = pbs;
        t->m_bitOffset = offset;
        return(FALSE);
    }

} /* CBaseBitstream::SearchBits_IgnoreOnly0() */


