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


#ifndef  _CALIBP_DEFAULTS_H_
#define  _CALIBP_DEFAULTS_H_


//  --------------------------------------------------
//  ----- Common calibration parmeters.

#ifdef CHIP_HAS_AP
#define CALIB_AUDIO_DEFAULT_VERSION     0XCAAD0101

#if (CALIB_AUDIO_DEFAULT_VERSION != CALIB_AUDIO_VERSION_NUMBER)
///the smart phone develop branch use the calibp_audio_defaults_dev.h
#include "calibp_audio_defaults_dev.h"
#else
///the smart phone release branch use the calibp_audio_defaults.h
#include "calibp_audio_defaults.h"
#endif

#else///!CHIP_HAS_AP
///the feature phone release branch and develop branch use the calibp_audio_defaults.h both
#include "calibp_audio_defaults.h"
#endif ///CHIP_HAS_AP


///  GP ADC params, sensor A
///  Value 0 indicates an error when using the default values
#define DEFAULT_GPADC_SENSOR_A              (0) //(0x271)
///  GP ADC params, sensor B
#define DEFAULT_GPADC_SENSOR_B              (0) //(0x309)

///  GP ADC default calib.
#define DEFAULT_CALIB_GPADC                 { DEFAULT_GPADC_SENSOR_A,\
                                              DEFAULT_GPADC_SENSOR_B }

///  RF analog params.
#define DEFAULT_TX_REF_VOLT                 0x0
///  RF analog params.
#define DEFAULT_TX_DAC_CURQ                 0x0
///  RF analog params.
#define DEFAULT_TX_DAC_CURI                 0x0
///  RF analog params.
#define DEFAULT_RX_REF_CAL                  0x3

/// @todo DON'T do that... Temporary things to get hal to compile
//  --------------------------------------------------
//  ----- Default calibration values.

// Default calibration times.

//  Tx Up
#define DEFAULT_CALIB_GMSK_UP               -16
#define DEFAULT_CALIB_DAC_UP                -316
#define DEFAULT_CALIB_TX_OEN_UP             -216

//  Tx Down
#define DEFAULT_CALIB_GMSK_DN               0
#define DEFAULT_CALIB_DAC_DN                30
#define DEFAULT_CALIB_TX_OEN_DN             15
#define DEFAULT_CALIB_DTX_OFF_DN            8

//  Rx Up
#define DEFAULT_CALIB_RF_IN_UP              0
#define DEFAULT_CALIB_ADC_UP                -200
#define DEFAULT_CALIB_FIRST_GAIN            -100
#define DEFAULT_CALIB_NEXT_GAIN             4

//  Rx Down
#define DEFAULT_CALIB_RF_IN_DN              0
#define DEFAULT_CALIB_ADC_DN                1

#define DEFAULT_CALIB_PAL_TIMES         {\
                                          DEFAULT_CALIB_GMSK_UP, \
                                          DEFAULT_CALIB_DAC_UP, \
                                          DEFAULT_CALIB_TX_OEN_UP, \
                                         \
                                          DEFAULT_CALIB_GMSK_DN, \
                                          DEFAULT_CALIB_DAC_DN, \
                                          DEFAULT_CALIB_TX_OEN_DN, \
                                          DEFAULT_CALIB_DTX_OFF_DN, \
                                          \
                                          DEFAULT_CALIB_RF_IN_UP, \
                                          DEFAULT_CALIB_ADC_UP, \
                                          DEFAULT_CALIB_FIRST_GAIN, \
                                          DEFAULT_CALIB_NEXT_GAIN, \
                                          \
                                          DEFAULT_CALIB_RF_IN_DN, \
                                          DEFAULT_CALIB_ADC_DN }

#endif //  CALIB_DEFAULTS_H

