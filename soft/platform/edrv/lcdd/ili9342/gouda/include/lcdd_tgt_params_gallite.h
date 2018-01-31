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

#ifndef TGT_LCDD_CONFIG

#define LCDD_CONFIG {                                                   \
    {.cs            =   HAL_GOUDA_LCD_CS_0,                             \
    .outputFormat   =   HAL_GOUDA_LCD_OUTPUT_FORMAT_8_bit_RGB565,      \
    .cs0Polarity    =   FALSE,                                          \
    .cs1Polarity    =   FALSE,                                          \
    .rsPolarity     =   FALSE,                                          \
    .wrPolarity     =   FALSE,                                          \
    .rdPolarity     =   FALSE}}

///@todo check bellow timings with LCD spec

// Write:
// tas >= 10 ns
// tah >=  5 ns
// pwl >= 50 ns
// pwh >= 50 ns

#define LCDD_TIMING_32K {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  1,                                                   \
    .pwh        =  3}}                                                  \


#define LCDD_TIMING_13M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  1,                                                   \
    .pwh        =  3}}                                                  \

#define LCDD_TIMING_26M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  2,                                                   \
    .pwh        =  3}}                                                  \

#define LCDD_TIMING_39M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  2,                                                   \
    .pwh        =  3}}                                                  \

#define LCDD_TIMING_52M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define LCDD_TIMING_78M {                                               \
    {.tas       =  2,                                                   \
    .tah        =  2,                                                   \
    .pwl        =  5,                                                   \
    .pwh        =  5}}                                                  \

#define LCDD_TIMING_104M {                                              \
    {.tas       =  2,                                                   \
    .tah        =  2,                                                   \
    .pwl        =  5,                                                   \
    .pwh        =  5}}                                                  \

#define LCDD_TIMING_156M {                                              \
    {.tas       =  4,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  9,                                                   \
    .pwh        =  9}}                                                  \

#define TGT_LCDD_CONFIG                                                 \
    {                                                                   \
        .config     = LCDD_CONFIG,                                      \
        .timings    = {                                                 \
            LCDD_TIMING_32K,                                            \
            LCDD_TIMING_13M,                                            \
            LCDD_TIMING_26M,                                            \
            LCDD_TIMING_39M,                                            \
            LCDD_TIMING_52M,                                            \
            LCDD_TIMING_78M,                                            \
            LCDD_TIMING_104M,                                           \
            LCDD_TIMING_156M,                                           \
        }                                                               \
    }

// Read:
// tas >=   5 ns
// tah >=   5 ns
// pwl >= 150 ns
// pwh >= 150 ns

#define LCDD_TIMING_READ_32K {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \


#define LCDD_TIMING_READ_13M {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_26M {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_39M {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_52M {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_78M {                                          \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_104M {                                         \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define LCDD_TIMING_READ_156M {                                         \
    {.tas       =  2,                                                   \
    .tah        =  4,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  50}}                                                 \


#define LCDD_READ_CONFIG                                               \
  {                                                                     \
     .config  =  LCDD_CONFIG,                                           \
     .timings    = {                                                    \
            LCDD_TIMING_READ_32K,                                       \
            LCDD_TIMING_READ_13M,                                       \
            LCDD_TIMING_READ_26M,                                       \
            LCDD_TIMING_READ_39M,                                       \
            LCDD_TIMING_READ_52M,                                       \
            LCDD_TIMING_READ_78M,                                       \
            LCDD_TIMING_READ_104M,                                      \
            LCDD_TIMING_READ_156M,                                      \
            }                                                           \
    }

#endif // TGT_LCDD_CONFIG

