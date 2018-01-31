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





#ifndef _BITSTREAM_H
#define _BITSTREAM_H

#include "aaccommon.h"

/* additional external symbols to name-mangle for static linking */
//#define SetBitstreamPointer               STATNAME(SetBitstreamPointer)
//#define AacGetBits                            STATNAME(AacGetBits)
//#define GetBitsNoAdvance              STATNAME(GetBitsNoAdvance)
//#define AdvanceBitstream              STATNAME(AdvanceBitstream)
//#define CalcBitsUsed                  STATNAME(CalcBitsUsed)
//#define ByteAlignBitstream                STATNAME(ByteAlignBitstream)

typedef struct _BitStreamInfo
{
    unsigned char *bytePtr;
    unsigned int iCache;
    int cachedBits;
    int nBytes;
} BitStreamInfo;

/* bitstream.c */
void SetBitstreamPointer(BitStreamInfo *bsi, int nBytes, unsigned char *buf);
unsigned int AacGetBits(BitStreamInfo *bsi, int nBits);
unsigned int GetBitsNoAdvance(BitStreamInfo *bsi, int nBits);
void AdvanceBitstream(BitStreamInfo *bsi, int nBits);
int CalcBitsUsed(BitStreamInfo *bsi, unsigned char *startBuf, int startOffset);
void ByteAlignBitstream(BitStreamInfo *bsi);

#endif  /* _BITSTREAM_H */
