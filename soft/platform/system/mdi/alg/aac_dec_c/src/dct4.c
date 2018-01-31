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





#include "aac_coder.h"
#include "aac_assembly.h"

static const int nmdctTab[NUM_IMDCT_SIZES] = {128, 1024};
static const int postSkip[NUM_IMDCT_SIZES] = {15, 1};

/**************************************************************************************
 * Function:    PreMultiply
 *
 * Description: pre-twiddle stage of DCT4
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmdct samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out, gains 5 (short) or 8 (long) frac bits
 *              i.e. gains 2-7= -5 int bits (short) or 2-10 = -8 int bits (long)
 *              normalization by -1/N is rolled into tables here (see trigtabs.c)
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 **************************************************************************************/
static void PreMultiply(int tabidx, int *zbuf1)
{
    int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
    int t, cms2, cps2a, sin2a, cps2b, sin2b;
    int *zbuf2;
    const int *csptr;

    nmdct = nmdctTab[tabidx];
    zbuf2 = zbuf1 + nmdct - 1;
    csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

    /* whole thing should fit in registers - verify that compiler does this */
    for (i = nmdct >> 2; i != 0; i--)
    {
        /* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
        cps2a = *csptr++;
        sin2a = *csptr++;
        cps2b = *csptr++;
        sin2b = *csptr++;

        ar1 = *(zbuf1 + 0);
        ai2 = *(zbuf1 + 1);
        ai1 = *(zbuf2 + 0);
        ar2 = *(zbuf2 - 1);

        /* gain 2 ints bit from MULSHIFT32 by Q30, but drop 7 or 10 int bits from table scaling of 1/M
         * max per-sample gain (ignoring implicit scaling) = MAX(sin(angle)+cos(angle)) = 1.414
         * i.e. gain 1 GB since worst case is sin(angle) = cos(angle) = 0.707 (Q30), gain 2 from
         *   extra sign bits, and eat one in adding
         */
        t  = MULSHIFT32(sin2a, ar1 + ai1);
        z2 = MULSHIFT32(cps2a, ai1) - t;
        cms2 = cps2a - 2*sin2a;
        z1 = MULSHIFT32(cms2, ar1) + t;
        *zbuf1++ = z1;  /* cos*ar1 + sin*ai1 */
        *zbuf1++ = z2;  /* cos*ai1 - sin*ar1 */

        t  = MULSHIFT32(sin2b, ar2 + ai2);
        z2 = MULSHIFT32(cps2b, ai2) - t;
        cms2 = cps2b - 2*sin2b;
        z1 = MULSHIFT32(cms2, ar2) + t;
        *zbuf2-- = z2;  /* cos*ai2 - sin*ar2 */
        *zbuf2-- = z1;  /* cos*ar2 + sin*ai2 */
    }
}

/**************************************************************************************
 * Function:    PostMultiply
 *
 * Description: post-twiddle stage of DCT4
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmdct samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out - gains 2 int bits
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 **************************************************************************************/
static void PostMultiply(int tabidx, int *fft1)
{
    int i, nmdct, ar1, ai1, ar2, ai2, skipFactor;
    int t, cms2, cps2, sin2;
    int *fft2;
    const int *csptr;

    nmdct = nmdctTab[tabidx];
    csptr = cos1sin1tab;
    skipFactor = postSkip[tabidx];
    fft2 = fft1 + nmdct - 1;

    /* load coeffs for first pass
     * cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin)
     */
    cps2 = *csptr++;
    sin2 = *csptr;
    csptr += skipFactor;
    cms2 = cps2 - 2*sin2;

    for (i = nmdct >> 2; i != 0; i--)
    {
        ar1 = *(fft1 + 0);
        ai1 = *(fft1 + 1);
        ar2 = *(fft2 - 1);
        ai2 = *(fft2 + 0);

        /* gain 2 ints bit from MULSHIFT32 by Q30
         * max per-sample gain = MAX(sin(angle)+cos(angle)) = 1.414
         * i.e. gain 1 GB since worst case is sin(angle) = cos(angle) = 0.707 (Q30), gain 2 from
         *   extra sign bits, and eat one in adding
         */
        t = MULSHIFT32(sin2, ar1 + ai1);
        *fft2-- = t - MULSHIFT32(cps2, ai1);    /* sin*ar1 - cos*ai1 */
        *fft1++ = t + MULSHIFT32(cms2, ar1);    /* cos*ar1 + sin*ai1 */
        cps2 = *csptr++;
        sin2 = *csptr;
        csptr += skipFactor;

        ai2 = -ai2;
        t = MULSHIFT32(sin2, ar2 + ai2);
        *fft2-- = t - MULSHIFT32(cps2, ai2);    /* sin*ar1 - cos*ai1 */
        cms2 = cps2 - 2*sin2;
        *fft1++ = t + MULSHIFT32(cms2, ar2);    /* cos*ar1 + sin*ai1 */
    }
}

/**************************************************************************************
 * Function:    PreMultiplyRescale
 *
 * Description: pre-twiddle stage of DCT4, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmdct samples
 *              number of guard bits to add to input before processing
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       see notes on PreMultiply(), above
 **************************************************************************************/
static void PreMultiplyRescale(int tabidx, int *zbuf1, int es)
{
    int i, nmdct, ar1, ai1, ar2, ai2, z1, z2;
    int t, cms2, cps2a, sin2a, cps2b, sin2b;
    int *zbuf2;
    const int *csptr;

    nmdct = nmdctTab[tabidx];
    zbuf2 = zbuf1 + nmdct - 1;
    csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

    /* whole thing should fit in registers - verify that compiler does this */
    for (i = nmdct >> 2; i != 0; i--)
    {
        /* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
        cps2a = *csptr++;
        sin2a = *csptr++;
        cps2b = *csptr++;
        sin2b = *csptr++;

        ar1 = *(zbuf1 + 0) >> es;
        ai1 = *(zbuf2 + 0) >> es;
        ai2 = *(zbuf1 + 1) >> es;

        t  = MULSHIFT32(sin2a, ar1 + ai1);
        z2 = MULSHIFT32(cps2a, ai1) - t;
        cms2 = cps2a - 2*sin2a;
        z1 = MULSHIFT32(cms2, ar1) + t;
        *zbuf1++ = z1;
        *zbuf1++ = z2;

        ar2 = *(zbuf2 - 1) >> es;   /* do here to free up register used for es */

        t  = MULSHIFT32(sin2b, ar2 + ai2);
        z2 = MULSHIFT32(cps2b, ai2) - t;
        cms2 = cps2b - 2*sin2b;
        z1 = MULSHIFT32(cms2, ar2) + t;
        *zbuf2-- = z2;
        *zbuf2-- = z1;

    }
}

/**************************************************************************************
 * Function:    PostMultiplyRescale
 *
 * Description: post-twiddle stage of DCT4, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmdct samples
 *              number of guard bits to remove from output
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       clips output to [-2^30, 2^30 - 1], guaranteeing at least 1 guard bit
 *              see notes on PostMultiply(), above
 **************************************************************************************/
static void PostMultiplyRescale(int tabidx, int *fft1, int es)
{
    int i, nmdct, ar1, ai1, ar2, ai2, skipFactor, z;
    int t, cs2, sin2;
    int *fft2;
    const int *csptr;

    nmdct = nmdctTab[tabidx];
    csptr = cos1sin1tab;
    skipFactor = postSkip[tabidx];
    fft2 = fft1 + nmdct - 1;

    /* load coeffs for first pass
     * cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin)
     */
    cs2 = *csptr++;
    sin2 = *csptr;
    csptr += skipFactor;

    for (i = nmdct >> 2; i != 0; i--)
    {
        ar1 = *(fft1 + 0);
        ai1 = *(fft1 + 1);
        ai2 = *(fft2 + 0);

        t = MULSHIFT32(sin2, ar1 + ai1);
        z = t - MULSHIFT32(cs2, ai1);
        CLIP_2N_SHIFT(z, es);
        *fft2-- = z;
        cs2 -= 2*sin2;
        z = t + MULSHIFT32(cs2, ar1);
        CLIP_2N_SHIFT(z, es);
        *fft1++ = z;

        cs2 = *csptr++;
        sin2 = *csptr;
        csptr += skipFactor;

        ar2 = *fft2;
        ai2 = -ai2;
        t = MULSHIFT32(sin2, ar2 + ai2);
        z = t - MULSHIFT32(cs2, ai2);
        CLIP_2N_SHIFT(z, es);
        *fft2-- = z;
        cs2 -= 2*sin2;
        z = t + MULSHIFT32(cs2, ar2);
        CLIP_2N_SHIFT(z, es);
        *fft1++ = z;
        cs2 += 2*sin2;
    }
}

/**************************************************************************************
 * Function:    DCT4
 *
 * Description: type-IV DCT
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmdct samples
 *              number of guard bits in the input buffer
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       operates in-place
 *              if number of guard bits in input is < GBITS_IN_DCT4, the input is
 *                scaled (>>) before the DCT4 and rescaled (<<, with clipping) after
 *                the DCT4 (rare)
 *              the output has FBITS_LOST_DCT4 fewer fraction bits than the input
 *              the output will always have at least 1 guard bit (GBITS_IN_DCT4 >= 4)
 *              int bits gained per stage (PreMul + FFT + PostMul)
 *                 short blocks = (-5 + 4 + 2) = 1 total
 *                 long blocks =  (-8 + 7 + 2) = 1 total
 **************************************************************************************/
void DCT4(int tabidx, int *coef, int gb)
{
    int es;

    /* fast in-place DCT-IV - adds guard bits if necessary */
    if (gb < GBITS_IN_DCT4)
    {
        es = GBITS_IN_DCT4 - gb;
        PreMultiplyRescale(tabidx, coef, es);
        R4FFT(tabidx, coef);
        PostMultiplyRescale(tabidx, coef, es);
    }
    else
    {
        PreMultiply(tabidx, coef);
        R4FFT(tabidx, coef);
        PostMultiply(tabidx, coef);
    }
}
