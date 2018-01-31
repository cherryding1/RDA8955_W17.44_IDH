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
 *   e.g. void FFT(int *fftbuf) becomes void cook_FFT(int *fftbuf)
 */
#define STAT_PREFIX     cook

#define STATCC1(x,y,z)  STATCC2(x,y,z)
#define STATCC2(x,y,z)  x##y##z

#ifdef STAT_PREFIX
#define STATNAME(func)  STATCC1(STAT_PREFIX, _, func)
#else
#define STATNAME(func)  func
#endif

/* global functions */
#define AllocateBuffers     STATNAME(AllocateBuffers)
#define FreeBuffers         STATNAME(FreeBuffers)

#define DecodeSideInfo      STATNAME(DecodeSideInfo)
#define GetBits             STATNAME(GetBits)
#define AdvanceBitstream    STATNAME(AdvanceBitstream)

#define DecodeHuffmanScalar STATNAME(DecodeHuffmanScalar)

#define DecodeGainInfo      STATNAME(DecodeGainInfo)
#define CopyGainInfo        STATNAME(CopyGainInfo)

#define JointDecodeMLT      STATNAME(JointDecodeMLT)
#define DecodeCoupleInfo    STATNAME(DecodeCoupleInfo)

#define DecodeEnvelope      STATNAME(DecodeEnvelope)
#define CategorizeAndExpand STATNAME(CategorizeAndExpand)
#define DecodeTransform     STATNAME(DecodeTransform)

#define IMLTNoWindow        STATNAME(IMLTNoWindow)
#define R4FFT               STATNAME(R4FFT)

#define DecWindowWithAttacks STATNAME(DecWindowWithAttacks)
#define DecWindowNoAttacks  STATNAME(DecWindowNoAttacks)

/* global (const) data */
#define pkkey               STATNAME(pkkey)

#define huffTabCoupleInfo   STATNAME(huffTabCoupleInfo)
#define huffTabCouple       STATNAME(huffTabCouple)
#define huffTabPowerInfo    STATNAME(huffTabPowerInfo)
#define huffTabPower        STATNAME(huffTabPower)
#define huffTabVectorInfo   STATNAME(huffTabVectorInfo)
#define huffTabVector       STATNAME(huffTabVector)

#define nmltTab             STATNAME(nmltTab)
#define window              STATNAME(window)
#define windowOffset        STATNAME(windowOffset)
#define cos4sin4tab         STATNAME(cos4sin4tab)
#define cos4sin4tabOffset   STATNAME(cos4sin4tabOffset)
#define cos1sin1tab         STATNAME(cos1sin1tab)
#define uniqueIDTab         STATNAME(uniqueIDTab)
#define bitrevtab           STATNAME(bitrevtab)
#define bitrevtabOffset     STATNAME(bitrevtabOffset)
#define twidTabEven         STATNAME(twidTabEven)
#define twidTabOdd          STATNAME(twidTabOdd)

/* assembly functions - either inline or in separate asm file */
#define MULSHIFT32          STATNAME(MULSHIFT32)
#define CLIPTOSHORT         STATNAME(CLIPTOSHORT)
#define FASTABS             STATNAME(FASTABS)
#define CLZ                 STATNAME(CLZ)

#endif  /* _STATNAME_H */
