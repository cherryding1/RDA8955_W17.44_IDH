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

static const int postSkip[NUM_MLT_SIZES] = {7, 3, 1};

/**************************************************************************************
 * Function:    PreMultiply
 *
 * Description: pre-twiddle stage of MDCT
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out - loses (1+tabidx) int bits
 *              normalization by 2/sqrt(N) is rolled into tables here
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 *              should asm code (compiler not doing free pointer updates, etc.)
 **************************************************************************************/
static void PreMultiply(int tabidx, int *zbuf1)
{
    int i, nmlt, ar1, ai1, ar2, ai2, z1, z2;
    int t, cms2, cps2a, sin2a, cps2b, sin2b;
    int *zbuf2;
    const int *csptr;

    nmlt = nmltTab[tabidx];
    zbuf2 = zbuf1 + nmlt - 1;
    csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

    /* whole thing should fit in registers - verify that compiler does this */
    for (i = nmlt >> 2; i != 0; i--)
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

        /* gain 1 int bit from MULSHIFT32, but drop 2, 3, or 4 int bits from table scaling */
        MULSHIFT32hx(sin2a, ar1 + ai1,t);
        z2 = MULSHIFT32(cps2a, ai1) - t;
        cms2 = cps2a - 2*sin2a;
        z1 = MULSHIFT32(cms2, ar1) + t;
        *zbuf1++ = z1;
        *zbuf1++ = z2;

        MULSHIFT32hx(sin2b, ar2 + ai2,t);
        z2 = MULSHIFT32(cps2b, ai2) - t;
        cms2 = cps2b - 2*sin2b;
        z1 = MULSHIFT32(cms2, ar2) + t;
        *zbuf2-- = z2;
        *zbuf2-- = z1;

    }

    /* Note on scaling...
     * assumes 1 guard bit in, gains (1 + tabidx) fraction bits
     *   i.e. gain 1, 2, or 3 fraction bits, for nSamps = 256, 512, 1024
     *   (left-shifting, since table scaled by 2 / sqrt(nSamps))
     * this offsets the fact that each extra pass in FFT gains one more int bit
     */
    return;
}

/**************************************************************************************
 * Function:    PostMultiply
 *
 * Description: post-twiddle stage of MDCT
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out - gains 1 int bit
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 *              should asm code (compiler not doing free pointer updates, etc.)
 **************************************************************************************/
static void PostMultiply(int tabidx, int *fft1)
{
    int i, nmlt, ar1, ai1, ar2, ai2, skipFactor, f0;
    int t, cms2, cps2, sin2;
    int *fft2;
    const int *csptr;

    nmlt = nmltTab[tabidx];
    csptr = cos1sin1tab;
    skipFactor = postSkip[tabidx];
    fft2 = fft1 + nmlt - 1;

    /* load coeffs for first pass
     * cps2 = (cos+sin)/2, sin2 = sin/2, cms2 = (cos-sin)/2
     */
    cps2 = *csptr++;
    sin2 = *csptr;
    csptr += skipFactor;
    cms2 = cps2 - 2*sin2;

    for (i = nmlt >> 2; i != 0; i--)
    {
        ar1 = *(fft1 + 0);
        ai1 = *(fft1 + 1);
        ar2 = *(fft2 - 1);
        ai2 = *(fft2 + 0);

        /* gain 1 int bit from MULSHIFT32, and one since coeffs are stored as 0.5 * (cos+sin), 0.5*sin */
        MULSHIFT32hx(sin2, ar1 + ai1,t);
        *fft2-- = t - MULSHIFT32(cps2, ai1);
        MULSHIFT32hx(cms2, ar1, f0);
        *fft1++ = t + f0;

        cps2 = *csptr++;
        sin2 = *csptr;
        csptr += skipFactor;

        ai2 = -ai2;
        MULSHIFT32hx(sin2, ar2 + ai2,t);
        *fft2-- = t - MULSHIFT32(cps2, ai2);
        cms2 = cps2 - 2*sin2;
        MULSHIFT32hx(cms2, ar2, f0);
        *fft1++ = t + f0;

    }

    /* Note on scaling...
     * assumes 1 guard bit in, gains 2 int bits
     * max gain of abs(cos) + abs(sin) = sqrt(2) = 1.414, so current format
     *   guarantees 1 guard bit in output
     */
    return;
}

/**************************************************************************************
 * Function:    PreMultiplyRescale
 *
 * Description: pre-twiddle stage of MDCT, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
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
    int i, nmlt, ar1, ai1, ar2, ai2, z1, z2;
    int t, cms2, cps2a, sin2a, cps2b, sin2b;
    int *zbuf2;
    const int *csptr;

    nmlt = nmltTab[tabidx];
    zbuf2 = zbuf1 + nmlt - 1;
    csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

    /* whole thing should fit in registers - verify that compiler does this */
    for (i = nmlt >> 2; i != 0; i--)
    {
        /* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
        cps2a = *csptr++;
        sin2a = *csptr++;
        cps2b = *csptr++;
        sin2b = *csptr++;

        ar1 = *(zbuf1 + 0) >> es;
        ai1 = *(zbuf2 + 0) >> es;
        ai2 = *(zbuf1 + 1) >> es;

        /* gain 1 int bit from MULSHIFT32, but drop 2, 3, or 4 int bits from table scaling */
        MULSHIFT32hx(sin2a, ar1 + ai1,t);
        z2 = MULSHIFT32(cps2a, ai1) - t;
        cms2 = cps2a - 2*sin2a;
        z1 = MULSHIFT32(cms2, ar1) + t;
        *zbuf1++ = z1;
        *zbuf1++ = z2;

        ar2 = *(zbuf2 - 1) >> es;   /* do here to free up register used for es */

        MULSHIFT32hx(sin2b, ar2 + ai2,t);
        z2 = MULSHIFT32(cps2b, ai2) - t;
        cms2 = cps2b - 2*sin2b;
        z1 = MULSHIFT32(cms2, ar2) + t;
        *zbuf2-- = z2;
        *zbuf2-- = z1;

    }

    /* see comments in PreMultiply() for notes on scaling */
    return;
}

/**************************************************************************************
 * Function:    PostMultiplyRescale
 *
 * Description: post-twiddle stage of MDCT, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
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
    int i, nmlt, ar1, ai1, ar2, ai2, skipFactor, z, f0;
    int t, cs2, sin2;
    int *fft2;
    const int *csptr;

    nmlt = nmltTab[tabidx];
    csptr = cos1sin1tab;
    skipFactor = postSkip[tabidx];
    fft2 = fft1 + nmlt - 1;

    /* load coeffs for first pass
     * cps2 = (cos+sin)/2, sin2 = sin/2, cms2 = (cos-sin)/2
     */
    cs2 = *csptr++;
    sin2 = *csptr;
    csptr += skipFactor;

    for (i = nmlt >> 2; i != 0; i--)
    {
        ar1 = *(fft1 + 0);
        ai1 = *(fft1 + 1);
        ai2 = *(fft2 + 0);

        /* gain 1 int bit from MULSHIFT32, and one since coeffs are stored as 0.5 * (cos+sin), 0.5*sin */
        MULSHIFT32hx(sin2, ar1 + ai1,t);
        z = t - MULSHIFT32(cs2, ai1);
        CLIP_2N_SHIFT(z, es);
        *fft2-- = z;
        cs2 -= 2*sin2;
        MULSHIFT32hx(cs2, ar1, f0);
        z = t + f0;
        CLIP_2N_SHIFT(z, es);
        *fft1++ = z;

        cs2 = *csptr++;
        sin2 = *csptr;
        csptr += skipFactor;

        ar2 = *fft2;
        ai2 = -ai2;
        MULSHIFT32hx(sin2, ar2 + ai2,t);
        z = t - MULSHIFT32(cs2, ai2);
        CLIP_2N_SHIFT(z, es);
        *fft2-- = z;
        cs2 -= 2*sin2;
        MULSHIFT32hx(cs2, ar2, f0);
        z = t + f0;
        CLIP_2N_SHIFT(z, es);
        *fft1++ = z;
        cs2 += 2*sin2;

    }

    /* see comments in PostMultiply() for notes on scaling */
    return;
}

/**************************************************************************************
 * Function:    IMLTNoWindow
 *
 * Description: inverse MLT without window or overlap-add
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *              number of guard bits in the input buffer
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       operates in-place, and generates nmlt output samples from nmlt input
 *                samples (doesn't do synthesis window which expands to 2*nmlt samples)
 *              if number of guard bits in input is < GBITS_IN_IMLT, the input is
 *                scaled (>>) before the IMLT and rescaled (<<, with clipping) after
 *                the IMLT (rare)
 *              the output has FBITS_LOST_IMLT fewer fraction bits than the input
 *              the output will always have at least 1 guard bit
 **************************************************************************************/
void IMLTNoWindow(int tabidx, int *mlt, int gb)
{
    int es;

    /* fast in-place DCT-IV - adds guard bits if necessary */
    if (gb < GBITS_IN_IMLT)
    {
        es = GBITS_IN_IMLT - gb;
        PreMultiplyRescale(tabidx, mlt, es);
        R4FFT(tabidx, mlt);
        PostMultiplyRescale(tabidx, mlt, es);
    }
    else
    {
        PreMultiply(tabidx, mlt);
        R4FFT(tabidx, mlt);
        PostMultiply(tabidx, mlt);
    }

    return;
}

