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





#include "aaccommon.h"

/* sample rates (table 4.5.1) */
const int aacSampRateTab[NUM_SAMPLE_RATES] =
{
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025,  8000
};

/* max scalefactor band for prediction (main profile only) */
const int predSFBMax[NUM_SAMPLE_RATES] =
{
    33, 33, 38, 40, 40, 40, 41, 41, 37, 37, 37, 34
};

/* channel mapping (table 1.6.3.4) (-1 = unknown, so need to determine mapping based on rules in 8.5.1) */
const int channelMapTab[NUM_DEF_CHAN_MAPS] =
{
    -1, 1, 2, 3, 4, 5, 6, 8
};

/* number of channels in each element (SCE, CPE, etc.)
 * see AACElementID in aaccommon.h
 */
const int elementNumChans[NUM_ELEMENTS] =
{
    1, 2, 0, 1, 0, 0, 0, 0
};

/* total number of scale factor bands in one window */
const unsigned char sfBandTotalShort[NUM_SAMPLE_RATES] =
{
    12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15
};

const unsigned char sfBandTotalLong[NUM_SAMPLE_RATES] =
{
    41, 41, 47, 49, 49, 51, 47, 47, 43, 43, 43, 40
};

/* scale factor band tables */
const int sfBandTabShortOffset[NUM_SAMPLE_RATES] = {0, 0, 0, 13, 13, 13, 28, 28, 44, 44, 44, 60};

const short sfBandTabShort[76] =
{
    /* short block 64, 88, 96 kHz [13] (tables 4.5.24, 4.5.26) */
    0,   4,   8,  12,  16,  20,  24,  32,  40,  48,  64,  92, 128,

    /* short block 32, 44, 48 kHz [15] (table 4.5.15) */
    0,   4,   8,  12,  16,  20,  28,  36,  44,  56,  68,  80,  96, 112, 128,

    /* short block 22, 24 kHz [16] (table 4.5.22) */
    0,   4,   8,  12,  16,  20,  24,  28,  36,  44,  52,  64,  76,  92, 108, 128,

    /* short block 11, 12, 16 kHz [16] (table 4.5.20) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  40,  48,  60,  72,  88, 108, 128,

    /* short block 8 kHz [16] (table 4.5.18) */
    0,   4,   8,  12,  16,  20,  24,  28,  36,  44,  52,  60,  72,  88, 108, 128
};

const int sfBandTabLongOffset[NUM_SAMPLE_RATES] = {0, 0, 42, 90, 90, 140, 192, 192, 240, 240, 240, 284};

const short sfBandTabLong[325] =
{
    /* long block 88, 96 kHz [42] (table 4.5.25) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,   52,
    56,  64,  72,  80,  88,  96, 108, 120, 132, 144, 156, 172, 188,  212,
    240, 276, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024,

    /* long block 64 kHz [48] (table 4.5.13) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  48,  52,  56,   64,
    72,  80,  88, 100, 112, 124, 140, 156, 172, 192, 216, 240, 268, 304, 344,  384,
    424, 464, 504, 544, 584, 624, 664, 704, 744, 784, 824, 864, 904, 944, 984, 1024,

    /* long block 44, 48 kHz [50] (table 4.5.14) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  48,  56,  64,  72,   80,  88,
    96, 108, 120, 132, 144, 160, 176, 196, 216, 240, 264, 292, 320, 352, 384,  416, 448,
    480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 1024,

    /* long block 32 kHz [52] (table 4.5.16) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  48,  56,  64,  72,   80,  88,  96,
    108, 120, 132, 144, 160, 176, 196, 216, 240, 264, 292, 320, 352, 384, 416,  448, 480, 512,
    544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992, 1024,

    /* long block 22, 24 kHz [48] (table 4.5.21) */
    0,   4,   8,  12,  16,  20,  24,  28,  32,  36,  40,  44,  52,  60,  68,   76,
    84,  92, 100, 108, 116, 124, 136, 148, 160, 172, 188, 204, 220, 240, 260,  284,
    308, 336, 364, 396, 432, 468, 508, 552, 600, 652, 704, 768, 832, 896, 960, 1024,

    /* long block 11, 12, 16 kHz [44] (table 4.5.19) */
    0,   8,  16,  24,  32,  40,  48,  56,  64,  72,  80,  88, 100,  112, 124,
    136, 148, 160, 172, 184, 196, 212, 228, 244, 260, 280, 300, 320,  344, 368,
    396, 424, 456, 492, 532, 572, 616, 664, 716, 772, 832, 896, 960, 1024,

    /* long block 8 kHz [41] (table 4.5.17) */
    0,  12,  24,  36,  48,  60,  72,  84,  96, 108, 120, 132,  144, 156,
    172, 188, 204, 220, 236, 252, 268, 288, 308, 328, 348, 372,  396, 420,
    448, 476, 508, 544, 580, 620, 664, 712, 764, 820, 880, 944, 1024
};


/* TNS max bands (table 4.139) and max order (table 4.138) */
const int tnsMaxBandsShortOffset[AAC_NUM_PROFILES] = {0, 0, 12};

const unsigned char tnsMaxBandsShort[2*NUM_SAMPLE_RATES] =
{
    9,  9, 10, 14, 14, 14, 14, 14, 14, 14, 14, 14,     /* short block, Main/LC */
    7,  7,  7,  6,  6,  6,  7,  7,  8,  8,  8,  7      /* short block, SSR */
};

const unsigned char tnsMaxOrderShort[AAC_NUM_PROFILES] = {7, 7, 7};

const int tnsMaxBandsLongOffset[AAC_NUM_PROFILES] = {0, 0, 12};

const unsigned char tnsMaxBandsLong[2*NUM_SAMPLE_RATES] =
{
    31, 31, 34, 40, 42, 51, 46, 46, 42, 42, 42, 39,     /* long block, Main/LC */
    28, 28, 27, 26, 26, 26, 29, 29, 23, 23, 23, 19,     /* long block, SSR */
};

const unsigned char tnsMaxOrderLong[AAC_NUM_PROFILES] = {20, 12, 12};
