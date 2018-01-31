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



# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "mp3_global.h"

# include <string.h>

# include "audio.h"
# include "mad.h"


//static struct audio_dither left_dither, right_dither;

static struct audio_dither dongwl_left_dither, dongwl_right_dither;



/*
 * NAME:    clip()
 * DESCRIPTION: gather signal statistics while clipping
 */
static inline
void clip(mad_fixed_t *sample, struct audio_stats *stats)
{
    enum
    {
        MIN = -MAD_F_ONE,
        MAX =  MAD_F_ONE - 1
    };

    if (*sample >= stats->peak_sample)
    {
        if (*sample > MAX)
        {
            ++stats->clipped_samples;
            if (*sample - MAX > stats->peak_clipping)
                stats->peak_clipping = *sample - MAX;

            *sample = MAX;
        }
        stats->peak_sample = *sample;
    }
    else if (*sample < -stats->peak_sample)
    {
        if (*sample < MIN)
        {
            ++stats->clipped_samples;
            if (MIN - *sample > stats->peak_clipping)
                stats->peak_clipping = MIN - *sample;

            *sample = MIN;
        }
        stats->peak_sample = -*sample;
    }
}


/*
 * NAME:    prng()
 * DESCRIPTION: 32-bit pseudo-random number generator
 */
static inline
unsigned long prng(unsigned long state)
{
    return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
 * NAME:    audio_linear_dither()
 * DESCRIPTION: generic linear sample quantize and dither routine
 */
static inline
signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample,
                                struct audio_dither *dither,
                                struct audio_stats *stats)
{
    unsigned int scalebits;
    mad_fixed_t output, mask, random;

    enum
    {
        MIN = -MAD_F_ONE,
        MAX =  MAD_F_ONE - 1
    };

    /* noise shape */
    sample += dither->error[0] - dither->error[1] + dither->error[2];

    dither->error[2] = dither->error[1];
    dither->error[1] = dither->error[0] / 2;

    /* bias */
    output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

    scalebits = MAD_F_FRACBITS + 1 - bits;
    mask = (1L << scalebits) - 1;

    /* dither */
    random  = prng(dither->random);
    output += (random & mask) - (dither->random & mask);

    dither->random = random;

    /* clip */
    if (output >= stats->peak_sample)
    {
        if (output > MAX)
        {
            ++stats->clipped_samples;
            if (output - MAX > stats->peak_clipping)
                stats->peak_clipping = output - MAX;

            output = MAX;

            if (sample > MAX)
                sample = MAX;
        }
        stats->peak_sample = output;
    }
    else if (output < -stats->peak_sample)
    {
        if (output < MIN)
        {
            ++stats->clipped_samples;
            if (MIN - output > stats->peak_clipping)
                stats->peak_clipping = MIN - output;

            output = MIN;

            if (sample < MIN)
                sample = MIN;
        }
        stats->peak_sample = -output;
    }

    /* quantize */
    output &= ~mask;

    /* error feedback */
    dither->error[0] = sample - output;

    /* scale */
    return output >> scalebits;
}


/*
 * NAME:    audio_pcm_s16le()
 * DESCRIPTION: write a block of signed 16-bit little-endian PCM samples
 */
unsigned int audio_pcm_s16le(unsigned char *data, unsigned int nsamples,
                             mad_fixed_t const *left, mad_fixed_t const *right,
                             struct audio_stats *pstats)
/*
{
    unsigned int len;
    register signed int sample0, sample1;

    struct audio_stats *stats=pstats;

    len = nsamples;

    while (len--) {
    sample0 = audio_linear_dither(16, *left++,  &dongwl_left_dither,  stats);
    sample1 = audio_linear_dither(16, *right++, &dongwl_right_dither, stats);

    data[0] = sample0 >> 0;
    data[1] = sample0 >> 8;
    data[2] = sample1 >> 0;
    data[3] = sample1 >> 8;

    data += 4;
      }

    return nsamples * 2 * 2;

}
*/
{
    unsigned int len;
    register signed int sample0, sample1;

    struct audio_stats *stats=pstats;

    len = nsamples;

    if(right)
    {

        while (len--)
        {
            sample0 = audio_linear_dither(16, *left++,  &dongwl_left_dither,  stats);
            sample1 = audio_linear_dither(16, *right++, &dongwl_right_dither, stats);

            data[0] = sample0 >> 0;
            data[1] = sample0 >> 8;
            data[2] = sample1 >> 0;
            data[3] = sample1 >> 8;

            data += 4;
        }

        return nsamples * 2 * 2;

    }
    else
    {

        while (len--)
        {
            sample0 = audio_linear_dither(16, *left++,  &dongwl_left_dither,  stats);
            data[0] = sample0 >> 0;
            data[1] = sample0 >> 8;
            data += 2;
        }

        return nsamples * 2;
    }

}


