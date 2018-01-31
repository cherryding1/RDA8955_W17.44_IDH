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



# ifndef LIBMAD_TIMER_H
# define LIBMAD_TIMER_H

typedef struct
{
    signed long seconds;      /* whole seconds */
    unsigned long fraction;   /* 1/MAD_TIMER_RESOLUTION seconds */
} mad_timer_t;

extern mad_timer_t const mad_timer_zero;

# define MAD_TIMER_RESOLUTION   352800000UL

enum mad_units
{
    MAD_UNITS_HOURS    =    -2,
    MAD_UNITS_MINUTES  =    -1,
    MAD_UNITS_SECONDS  =     0,

    /* metric units */

    MAD_UNITS_DECISECONDS  =    10,
    MAD_UNITS_CENTISECONDS =   100,
    MAD_UNITS_MILLISECONDS =  1000,

    /* audio sample units */

    MAD_UNITS_8000_HZ  =  8000,
    MAD_UNITS_11025_HZ     = 11025,
    MAD_UNITS_12000_HZ     = 12000,

    MAD_UNITS_16000_HZ     = 16000,
    MAD_UNITS_22050_HZ     = 22050,
    MAD_UNITS_24000_HZ     = 24000,

    MAD_UNITS_32000_HZ     = 32000,
    MAD_UNITS_44100_HZ     = 44100,
    MAD_UNITS_48000_HZ     = 48000,

    /* video frame/field units */

    MAD_UNITS_24_FPS   =    24,
    MAD_UNITS_25_FPS   =    25,
    MAD_UNITS_30_FPS   =    30,
    MAD_UNITS_48_FPS   =    48,
    MAD_UNITS_50_FPS   =    50,
    MAD_UNITS_60_FPS   =    60,

    /* CD audio frames */

    MAD_UNITS_75_FPS   =    75,

    /* video drop-frame units */

    MAD_UNITS_23_976_FPS   =   -24,
    MAD_UNITS_24_975_FPS   =   -25,
    MAD_UNITS_29_97_FPS    =   -30,
    MAD_UNITS_47_952_FPS   =   -48,
    MAD_UNITS_49_95_FPS    =   -50,
    MAD_UNITS_59_94_FPS    =   -60
};

# define mad_timer_reset(timer) ((void) (*(timer) = mad_timer_zero))

int mad_timer_compare(mad_timer_t, mad_timer_t);

# define mad_timer_sign(timer)  mad_timer_compare((timer), mad_timer_zero)

void mad_timer_negate(mad_timer_t *);
mad_timer_t mad_timer_abs(mad_timer_t);

void mad_timer_set(mad_timer_t *, unsigned long, unsigned long, unsigned long);
void mad_timer_add(mad_timer_t *, mad_timer_t);
void mad_timer_multiply(mad_timer_t *, signed long);

signed long mad_timer_count(mad_timer_t, enum mad_units);
unsigned long mad_timer_fraction(mad_timer_t, unsigned long);
void mad_timer_string(mad_timer_t, char *, char const *,
                      enum mad_units, enum mad_units, unsigned long);

# endif
