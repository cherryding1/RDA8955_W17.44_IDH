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





#ifndef _STATNAME_H
#define _STATNAME_H

/* define STAT_PREFIX to a unique name for static linking
 * all the C functions and global variables will be mangled by the preprocessor
 *   e.g. void DCT4(...) becomes void raac_DCT4(...)
 */
#define STAT_PREFIX     raac

#define STATCC1(x,y,z)  STATCC2(x,y,z)
#define STATCC2(x,y,z)  x##y##z

#ifdef STAT_PREFIX
#define STATNAME(func)  STATCC1(STAT_PREFIX, _, func)
#else
#define STATNAME(func)  func
#endif

/* these symbols are common to all implementations */
#define AllocateBuffers         STATNAME(AllocateBuffers)
#define FreeBuffers             STATNAME(FreeBuffers)
#define ClearBuffer             STATNAME(ClearBuffer)

#define SetRawBlockParams       STATNAME(SetRawBlockParams)
#define PrepareRawBlock         STATNAME(PrepareRawBlock)
#define FlushCodec              STATNAME(FlushCodec)

#define UnpackADTSHeader        STATNAME(UnpackADTSHeader)
#define GetADTSChannelMapping   STATNAME(GetADTSChannelMapping)
#define UnpackADIFHeader        STATNAME(UnpackADIFHeader)
#define DecodeNextElement       STATNAME(DecodeNextElement)
#define DecodeNoiselessData     STATNAME(DecodeNoiselessData)
#define Dequantize              STATNAME(Dequantize)
#define StereoProcess           STATNAME(StereoProcess)
#define DeinterleaveShortBlocks STATNAME(DeinterleaveShortBlocks)
#define PNS                     STATNAME(PNS)
#define TNSFilter               STATNAME(TNSFilter)
#define IMDCT                   STATNAME(aac_IMDCT)

#define InitSBR                 STATNAME(InitSBR)
#define DecodeSBRBitstream      STATNAME(DecodeSBRBitstream)
#define DecodeSBRData           STATNAME(DecodeSBRData)
#define FreeSBR                 STATNAME(FreeSBR)
#define FlushCodecSBR           STATNAME(FlushCodecSBR)

/* global ROM tables */
#define aacSampRateTab              STATNAME(aacSampRateTab)
#define predSFBMax              STATNAME(predSFBMax)
#define channelMapTab           STATNAME(channelMapTab)
#define elementNumChans         STATNAME(elementNumChans)
#define sfBandTotalShort        STATNAME(sfBandTotalShort)
#define sfBandTotalLong         STATNAME(sfBandTotalLong)
#define sfBandTabShortOffset    STATNAME(sfBandTabShortOffset)
#define sfBandTabShort          STATNAME(sfBandTabShort)
#define sfBandTabLongOffset     STATNAME(sfBandTabLongOffset)
#define sfBandTabLong           STATNAME(sfBandTabLong)
#define tnsMaxBandsShortOffset  STATNAME(tnsMaxBandsShortOffset)
#define tnsMaxBandsShort        STATNAME(tnsMaxBandsShort)
#define tnsMaxOrderShort        STATNAME(tnsMaxOrderShort)
#define tnsMaxBandsLongOffset   STATNAME(tnsMaxBandsLongOffset)
#define tnsMaxBandsLong         STATNAME(tnsMaxBandsLong)
#define tnsMaxOrderLong         STATNAME(tnsMaxOrderLong)

/* in your implementation's top-level include file (e.g. real\coder.h) you should
 *   add new #define sym STATNAME(sym) lines for all the
 *   additional global functions or variables which your
 *   implementation uses
 */

#endif /* _STATNAME_H */
