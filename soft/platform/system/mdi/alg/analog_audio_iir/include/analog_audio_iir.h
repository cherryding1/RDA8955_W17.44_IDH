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

#ifndef ANALOG_AUDIO_IIR_H
#define ANALOG_AUDIO_IIR_H

#ifdef ANDROID
#include <cutils/log.h>
#define IIRLOGI(msg, ...)   LOGI(msg, ##__VA_ARGS__)
#define IIRLOGE(msg, ...)   LOGE(msg, ##__VA_ARGS__)
#else
#include "mcip_debug.h"
#define DEBUG
#define IIRLOGI diag_printf
#define IIRLOGE diag_printf
#endif

#define FIXED_POINT

void calc_coeffs_for_voc(CALIB_AUDIO_IIR_EQ_T *eq, int samplerate, int itf);

void clean_iir_history(void);

void init_iir_history(int itf, int fs, signed char *gain, double *qual,
                      unsigned short *freq);

int iir(int itf, int type, short *data, int length, int nch, int clean_history);

#endif