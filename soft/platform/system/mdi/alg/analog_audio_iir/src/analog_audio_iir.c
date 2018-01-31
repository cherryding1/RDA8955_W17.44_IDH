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

#define LOG_TAG "analog_audio_iir"
#define LOG_NDEBUG 0

#if 0
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#endif
#include <string.h>
#include "fdlibm.h"
#include "calib_m.h"
#include "analog_audio_iir.h"

#ifdef FIXED_POINT
#include "fixed_generic.h"
#endif

#define PI 3.1415926
#define EQ_CHANNELS 2
#define EQ_MAX_BANDS 10
//#define CALIB_AUDIO_ITF_QTY 6

#ifdef FIXED_POINT
typedef short coeff_t;
typedef int data_t;

#define COEFF_SHIFT 14
#define DATA_SHIFT 13

#define CAT(a,...)  PRIMITIVE_CAT(a,__VA_ARGS__)
#define PRIMITIVE_CAT(a,...)    a ## __VA_ARGS__
#define MULT16_32_QCOEFF    CAT(MULT16_32_Q, COEFF_SHIFT)
#else
typedef double coeff_t;
typedef double data_t;

#define QCONST16(x,bits)    (x)
#define QCONST32(x,bits)    (x)
#define SHR32(a,shift)      (a)
#define PSHR32(a,shift)     (a)
#define MULT16_32_QCOEFF(a,b)   ((a)*(b))
#endif

#if 0
typedef enum
{
    SKIP = 0,
    LPF,
    HPF,
    peakingEQ,
    lowShelf,
    highShelf,
} filter_type;
#else
typedef FILTER_TYPE filter_type;
#endif

/*
 * 10 bands = 1 hpf(lowShelf) + 8 peakingEQ + 1 lpf(highShelf)
 * lowShelf and highShelf not working now
 */
filter_type filters[10] = {HPF, peakingEQ, peakingEQ, peakingEQ, peakingEQ,
                           peakingEQ, LPF, SKIP, SKIP, SKIP
                          };

typedef struct
{
    coeff_t num[3];
    coeff_t den[3];
} sIIRcoefficients;

typedef struct
{
    data_t x[3]; //x[n-2], x[n-1], x[n]
    data_t y[3]; //y[n-2], y[n-1], y[n]
} sXYData; //bytes; 4*3 + 4*3 = 24
// to work with VoC, struct x[3] = {x[n], x[n-1], x[n-2]} should be better

static sIIRcoefficients coeffs[CALIB_AUDIO_ITF_QTY][EQ_MAX_BANDS];

static int fs_bands = 7;

void get_coeffs_skip(int itf, signed char gain, double qual,
                     unsigned short freq, int fs, int band)
{
    coeffs[itf][band].num[0] = QCONST16(1.0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = 0;
    coeffs[itf][band].num[2] = 0;
    coeffs[itf][band].den[0] = QCONST16(1.0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = 0;
    coeffs[itf][band].den[2] = 0;
}

void get_coeffs_LPF(int itf, signed char gain, double qual,
                    unsigned short freq, int fs, int band)
{
    double alpha, w0;
    //double A = pow(10, (double)gain/40);
    double a0, a1, a2, b0, b1, b2;
    //double BW = 0.1;
    //double Q = 1;
    //double tmp;

    w0 = freq * 2 * PI / fs;
    alpha = sin(w0) / (2.0 * qual);
    // calc use BW
    //tmp = log(2)/2 * BW * w0/sin(w0);
    //alpha = sin(w0) * sinh(tmp);

    b0 = (1 - cos(w0)) / 2;
    b1 = 1 - cos(w0);
    b2 = (1 - cos(w0)) / 2;
    a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    coeffs[itf][band].num[0] = QCONST16(b0 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = QCONST16(b1 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[2] = QCONST16(b2 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[0] = QCONST16(a0 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = -QCONST16(a1 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[2] = -QCONST16(a2 / a0, COEFF_SHIFT);
}

void get_coeffs_HPF(int itf, signed char gain, double qual,
                    unsigned short freq, int fs, int band)
{
    double alpha, w0;
    //double A = pow(10, (double)gain/40);
    double a0, a1, a2, b0, b1, b2;
    //double BW = 0.1;
    //double Q = 1;
    //double tmp;

    w0 = freq * 2 * PI / fs;
    alpha = sin(w0) / (2.0 * qual);
    // calc use BW
    //tmp = log(2)/2 * BW * w0/sin(w0);
    //alpha = sin(w0) * sinh(tmp);

    b0 = (1 + cos(w0)) / 2;
    b1 = -(1 + cos(w0));
    b2 = (1 + cos(w0)) / 2;
    a0 = 1 + alpha;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha;

    coeffs[itf][band].num[0] = QCONST16(b0 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = QCONST16(b1 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[2] = QCONST16(b2 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[0] = QCONST16(a0 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = -QCONST16(a1 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[2] = -QCONST16(a2 / a0, COEFF_SHIFT);
}

void get_coeffs_peakingEQ(int itf, signed char gain, double qual,
                          unsigned short freq, int fs, int band)
{
    double alpha, w0;
    double A = pow(10, (double)gain/40);
    double a0, a1, a2, b0, b1, b2;
    //double BW = 0.1;
    //double Q = 1;
    //double tmp;

    w0 = freq * 2 * PI / fs;
    alpha = sin(w0) / (2.0 * qual);
    // calc use BW
    //tmp = log(2)/2 * BW * w0/sin(w0);
    //alpha = sin(w0) * sinh(tmp);

    b0 = 1 + alpha * A;
    b1 = -2 * cos(w0);
    b2 = 1 - alpha * A;
    a0 = 1 + alpha / A;
    a1 = -2 * cos(w0);
    a2 = 1 - alpha / A;

    coeffs[itf][band].num[0] = QCONST16(b0 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = QCONST16(b1 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[2] = QCONST16(b2 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[0] = QCONST16(a0 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = -QCONST16(a1 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[2] = -QCONST16(a2 / a0, COEFF_SHIFT);
}

/* there is still some issue with lowShelf and highShelf filters */
void get_coeffs_lowShelf(int itf, signed char gain, double qual,
                         unsigned short freq, int fs, int band)
{
    double alpha, w0;
    double A = pow(10, (double)gain/40);
    double a0, a1, a2, b0, b1, b2;
    //double BW = 0.1;
    //double Q = 1;
    //double tmp;

    w0 = freq * 2 * PI / fs;
    //alpha = sin(w0) / (2.0 * qual);
    alpha = sin(w0)/2 * sqrt( (A + 1 / A)*(10 / qual - 1) + 2 );
    // calc use BW
    //tmp = log(2)/2 * BW * w0/sin(w0);
    //alpha = sin(w0) * sinh(tmp);

    b0 = A * ((A + 1) - (A - 1) * cos(w0) + 2 * sqrt(A) * alpha);
    b1 = 2 * A * ((A - 1) - (A + 1) * cos(w0));
    b2 = A * ((A + 1) - (A - 1) * cos(w0) - 2 * sqrt(A) * alpha);
    a0 = (A + 1) + (A - 1) * cos(w0) + 2 * sqrt(A) * alpha;
    a1 = -2 * ((A - 1) +  (A + 1) * cos(w0));
    a2 = (A + 1) + (A - 1) * cos(w0) - 2 * sqrt(A) * alpha;

    coeffs[itf][band].num[0] = QCONST16(b0 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = QCONST16(b1 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[2] = QCONST16(b2 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[0] = QCONST16(a0 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = -QCONST16(a1 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[2] = -QCONST16(a2 / a0, COEFF_SHIFT);
}

void get_coeffs_highShelf(int itf, signed char gain, double qual,
                          unsigned short freq, int fs, int band)
{
    double alpha, w0;
    double A = pow(10, (double)gain/40);
    double a0, a1, a2, b0, b1, b2;
    //double BW = 0.1;
    //double Q = 1;
    //double tmp;

    w0 = freq * 2 * PI / fs;
    alpha = sin(w0) / (2.0 * qual);
    // calc use BW
    //tmp = log(2)/2 * BW * w0/sin(w0);
    //alpha = sin(w0) * sinh(tmp);

    b0 = A * ((A + 1) + (A - 1) * cos(w0) + 2 * sqrt(A) * alpha);
    b1 = -2 * A * ((A - 1) - (A + 1) * cos(w0));
    b2 = A * ((A + 1) + (A - 1) * cos(w0) - 2 * sqrt(A) * alpha);
    a0 = (A + 1) - (A - 1) * cos(w0) + 2 * sqrt(A) * alpha;
    a1 = 2 * ((A - 1) -  (A + 1) * cos(w0));
    a2 = (A + 1) - (A - 1) * cos(w0) - 2 * sqrt(A) * alpha;

    coeffs[itf][band].num[0] = QCONST16(b0 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[1] = QCONST16(b1 / a0, COEFF_SHIFT);
    coeffs[itf][band].num[2] = QCONST16(b2 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[0] = QCONST16(a0 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[1] = -QCONST16(a1 / a0, COEFF_SHIFT);
    coeffs[itf][band].den[2] = -QCONST16(a2 / a0, COEFF_SHIFT);
}

void get_coeffs_for_one_band(int itf, signed char gain, double qual,
                             unsigned short freq, int fs, int band, filter_type type)
{

    if (freq * 2 > fs)
        type = SKIP;

    switch(type)
    {
        case SKIP:
            get_coeffs_skip(itf, gain, qual, freq, fs, band);
            break;
        case LPF:
            get_coeffs_LPF(itf, gain, qual, freq, fs, band);
            break;
        case HPF:
            get_coeffs_HPF(itf, gain, qual, freq, fs, band);
            break;
        case peakingEQ:
            get_coeffs_peakingEQ(itf, gain, qual, freq, fs, band);
            break;
        case lowShelf:
            get_coeffs_lowShelf(itf, gain, qual, freq, fs, band);
            break;
        case highShelf:
            get_coeffs_highShelf(itf, gain, qual, freq, fs, band);
            break;
        default:
            IIRLOGE("audio-hal : current filter not implemented\n");
    }
}

void get_coeffs(int itf, signed char *gain, double *qual, unsigned short *freq, int fs)
{
    int ii = 0;
    //int alpha_num = 0;

    for (ii = 0; ii < fs_bands; ii++)
    {
        get_coeffs_for_one_band(itf, gain[ii], qual[ii], freq[ii], fs, ii, filters[ii]);

#ifndef FIXED_POINT
        IIRLOGI("%f, %f, %f, %f, %f, %f\n",
#else
        IIRLOGI("0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x\n",
#endif
                coeffs[itf][ii].num[0], coeffs[itf][ii].num[1], coeffs[itf][ii].num[2],
                coeffs[itf][ii].den[0], coeffs[itf][ii].den[1], coeffs[itf][ii].den[2]
               );
    }
}

void calc_coeffs_for_voc(CALIB_AUDIO_IIR_EQ_T *eq, int samplerate, int itf)
{
    int ii = 0;
    for (ii = 0; ii < fs_bands; ii++)
    {
        CALIB_AUDIO_IIR_EQ_BAND_T *band = &(eq->band[ii]);
        // FIXME: assume 7 is the normalized Q (sqrt(2)/2)
        double qual = band->param.qual / 7.0 * sqrt(2) / 2.0;

        get_coeffs_for_one_band(itf, band->gain, qual, band->freq, samplerate, ii, band->type);

        band->num[0] = coeffs[itf][ii].num[0];
        band->num[1] = coeffs[itf][ii].num[1];
        band->num[2] = coeffs[itf][ii].num[2];
        band->den[0] = coeffs[itf][ii].den[0];
        band->den[1] = coeffs[itf][ii].den[1];
        band->den[2] = coeffs[itf][ii].den[2];

#ifndef FIXED_POINT
        IIRLOGI("%f, %f, %f, %f, %f, %f\n",
#else
        IIRLOGI("0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x\n",
#endif
                band->num[0], band->num[1], band->num[2],
                band->den[0], band->den[1], band->den[2]
               );
    }
}

static sXYData data_history[EQ_MAX_BANDS][EQ_CHANNELS];

void clean_iir_history(void)
{
    memset(data_history, 0, sizeof(sXYData) * EQ_MAX_BANDS * EQ_CHANNELS);
}

void init_iir_history(int itf, int fs, signed char *gain, double *qual,
                      unsigned short *freq)
{
    get_coeffs(itf, gain, qual, freq, fs);
    /* clean history */
    clean_iir_history();
}

/* TODO: rewrite iir process using neon */
int iir(int itf, int type, short *data, int length, int nch, int clean_history)
{
    int index, band, channel, tempgint;
    //short pcm[EQ_CHANNELS];

    if (clean_history != 0)
    {
        clean_iir_history();
    }

    for (index = 0; index < length; index+=nch)
    {
        for (channel = 0; channel < nch; channel++)
        {
            int pcm = QCONST32(data[index+channel], DATA_SHIFT);

            for (band = 0; band < fs_bands; band++)
            {
                sXYData *history_data = &data_history[band][channel];
                sIIRcoefficients *coeff = &coeffs[itf][band];

                history_data->x[0] = history_data->x[1];
                history_data->x[1] = history_data->x[2];
                history_data->x[2] = pcm;

                history_data->y[0] = history_data->y[1];
                history_data->y[1] = history_data->y[2];
#ifndef DEBUG
                history_data->y[2] =
                    MULT16_32_QCOEFF(coeff->num[2], history_data->x[0])+
                    MULT16_32_QCOEFF(coeff->num[1], history_data->x[1])+
                    MULT16_32_QCOEFF(coeff->num[0], history_data->x[2])+
                    MULT16_32_QCOEFF(coeff->den[2], history_data->y[0])+
                    MULT16_32_QCOEFF(coeff->den[1], history_data->y[1]);
#else
                history_data->y[2] = MULT16_32_QCOEFF(coeff->num[1], history_data->x[1]);
                history_data->y[2] += MULT16_32_QCOEFF(coeff->num[0], history_data->x[2]);
                history_data->y[2] += MULT16_32_QCOEFF(coeff->num[2], history_data->x[0]);
                history_data->y[2] += MULT16_32_QCOEFF(coeff->den[2], history_data->y[0]);
                history_data->y[2] += MULT16_32_QCOEFF(coeff->den[1], history_data->y[1]);
#endif
                pcm = history_data->y[2];
            }

            tempgint = PSHR32(pcm, DATA_SHIFT);

            if (tempgint < -32768)
                data[index+channel] = -32768;
            else if (tempgint > 32767)
                data[index+channel] = 32767;
            else
                data[index+channel] = tempgint;
        }
    }
    return length;
}

