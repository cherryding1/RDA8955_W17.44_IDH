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



# ifndef AUDIO_H
# define AUDIO_H

# include "mad.h"

# define MAX_NSAMPLES   (1152 * 3)  /* allow for resampled frame */

struct audio_stats
{
    unsigned long clipped_samples;
    mad_fixed_t peak_clipping;
    mad_fixed_t peak_sample;
};

struct audio_dither
{
    mad_fixed_t error[3];
    mad_fixed_t random;
};


unsigned int audio_pcm_s16le(unsigned char *data, unsigned int nsamples,
                             mad_fixed_t const *left, mad_fixed_t const *right,
                             struct audio_stats *stats);



# endif
