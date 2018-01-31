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



























#ifndef DECDEFS_H__
#define DECDEFS_H__

#include "beginhdr.h"
#include "rvtypes.h"

/* This file defines some data structures and constants used by the decoder, */
/* that are also needed by other classes, such as post filters and */
/* error concealment. */

#define INTERP_FACTOR 4
#define INTERP_FACTOR_RV8 3
#define INTERP_ROUND  1




/* The decoder stores motion vectors for each luma 4x4 subblock in an */
/* entire picture. */
/* A motion vector is defined as 2 signed components in third-pel */
/* units - one for x and one for y. */

typedef I16 DecoderPackedMotionVectorComponent;
/* An I16 is sufficient to hold any subpel motion vector for */
/* our supported image dimensions. */
/* Note that "our" encoder currently only emits motion vectors that */
/* fit in an I8.  However, we use an I16 to allow decoding bitstreams */
/* from other sources, such as Telenor. */

struct DecoderPackedMotionVector
{
    DecoderPackedMotionVectorComponent  mvx;
    DecoderPackedMotionVectorComponent  mvy;
};


/* Provide a way to get at the motion vectors for subblock 0 of a */
/* macroblock, given the macroblock's pel position in the luma plane. */

#define getMBMotionVectors(pMVOrigin, mbXOffset, mbYOffset, MVPitch) \
    ((pMVOrigin) + ((mbXOffset) >> 2) + (MVPitch) * ((mbYOffset) >> 2))


/* The decoder stores intra-coding types (0..4) in U8's. */

typedef U8 DecoderIntraType;




/* */
/* Declare some reconstruction functionality used across several files. */
/* */

#ifdef __cplusplus
extern "C"
{
#endif

extern void
MV_Sub2FullPels(const I32 mv, I32* mv_intg, I32* mv_frac);
/* MV_Sub2FullPels extracts the integer and fractional components */
/* of a motion vector that's in subpel units. */

extern void
MV_Sub2FullPelsChromaYi(const I32 mv, I32* mv_intg, I32* mv_frac);
/* MV_Sub2FullPels extracts the integer and fractional components */
/* of a motion vector that's in subpel units. */

/* versions for B frames and B frames chroma, since these might want to do
   some shortcuts for CPU scalability */
extern void
MV_Sub2FullPels_B(const I32 mv, I32* mv_intg, I32* mv_frac);
extern void
MV_Sub2FullPels_BC(const I32 mv, I32* mv_intg, I32* mv_frac);

/* same as  MV_Sub2FullPels, but for RV8 (1/3 pel) */
extern void
MV_Sub2FullPels_RV8(const I32 mv, I32* mv_intg, I32* mv_frac);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* DECDEFS_H__ */
