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

#ifndef TGT_SUBLCDD_CONFIG

#define SUBLCDD_CONFIG {                                                   \
    {.cs            =   HAL_GOUDA_LCD_CS_1,                             \
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
// pwl >= 15 ns
// pwh >= 15 ns

#define SUBLCDD_TIMING_32K {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \


#define SUBLCDD_TIMING_13M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define SUBLCDD_TIMING_26M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define SUBLCDD_TIMING_39M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define SUBLCDD_TIMING_52M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define SUBLCDD_TIMING_78M {                                               \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  3,                                                   \
    .pwh        =  3}}                                                  \

#define SUBLCDD_TIMING_104M {                                              \
    {.tas       =  2,                                                   \
    .tah        =  2,                                                   \
    .pwl        =  6,                                                   \
    .pwh        =  6}}                                                  \

#define SUBLCDD_TIMING_156M {                                              \
    {.tas       =  2,                                                   \
    .tah        =  2,                                                   \
    .pwl        =  6,                                                   \
    .pwh        =  6}}                                                  \

#define TGT_SUBLCDD_CONFIG                                                 \
    {                                                                   \
        .config     = SUBLCDD_CONFIG,                                      \
        .timings    = {                                                 \
            SUBLCDD_TIMING_32K,                                            \
            SUBLCDD_TIMING_13M,                                            \
            SUBLCDD_TIMING_26M,                                            \
            SUBLCDD_TIMING_39M,                                            \
            SUBLCDD_TIMING_52M,                                            \
            SUBLCDD_TIMING_78M,                                            \
            SUBLCDD_TIMING_104M,                                           \
            SUBLCDD_TIMING_156M,                                           \
        }                                                               \
    }

// Read:
// tas >=   5 ns
// tah >=   5 ns
// pwl >= 150 ns
// pwh >= 150 ns

#define SUBLCDD_TIMING_READ_32K {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \


#define SUBLCDD_TIMING_READ_13M {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_26M {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_39M {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_52M {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_78M {                                          \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_104M {                                         \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \

#define SUBLCDD_TIMING_READ_156M {                                         \
    {.tas       =  1,                                                   \
    .tah        =  1,                                                   \
    .pwl        =  40,                                                  \
    .pwh        =  40}}                                                 \


#define SUBLCDD_READ_CONFIG                                               \
  {                                                                     \
     .config  =  SUBLCDD_CONFIG,                                           \
     .timings    = {                                                    \
            SUBLCDD_TIMING_READ_32K,                                       \
            SUBLCDD_TIMING_READ_13M,                                       \
            SUBLCDD_TIMING_READ_26M,                                       \
            SUBLCDD_TIMING_READ_39M,                                       \
            SUBLCDD_TIMING_READ_52M,                                       \
            SUBLCDD_TIMING_READ_78M,                                       \
            SUBLCDD_TIMING_READ_104M,                                      \
            SUBLCDD_TIMING_READ_156M,                                      \
            }                                                           \
    }


// =============================================================================
/// Serial LCDD config
// =============================================================================

#define LCDD_SPI_LINE_TYPE     (HAL_GOUDA_SPI_LINE_4)


#define LCDD_SPI_FREQ          (6000000)
#define LCDD_SPI_FREQ_READ     (500000)


#endif // TGT_SUBLCDD_CONFIG

