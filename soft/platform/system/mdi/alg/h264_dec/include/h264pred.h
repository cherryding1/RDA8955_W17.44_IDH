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





#ifndef FFMPEG_H264PRED_H
#define FFMPEG_H264PRED_H

#include "common.h"

/**
 * Prediction types
 */
//@{
#define VERT_PRED             0
#define HOR_PRED              1
#define DC_PRED               2
#define DIAG_DOWN_LEFT_PRED   3
#define DIAG_DOWN_RIGHT_PRED  4
#define VERT_RIGHT_PRED       5
#define HOR_DOWN_PRED         6
#define VERT_LEFT_PRED        7
#define HOR_UP_PRED           8

#define LEFT_DC_PRED          9
#define TOP_DC_PRED           10
#define DC_128_PRED           11

#define DIAG_DOWN_LEFT_PRED_RV40_NODOWN   12
#define HOR_UP_PRED_RV40_NODOWN           13
#define VERT_LEFT_PRED_RV40_NODOWN        14

#define DC_PRED8x8            0
#define HOR_PRED8x8           1
#define VERT_PRED8x8          2
#define PLANE_PRED8x8         3

#define LEFT_DC_PRED8x8       4
#define TOP_DC_PRED8x8        5
#define DC_128_PRED8x8        6
//@}

/**
 * Context for storing H.264 prediction functions
 */
typedef struct H264PredContext
{
    void (*pred4x4  [9+3+3])(uint8_t *src, uint8_t *topright, int stride);//FIXME move to dsp?
    void (*pred8x8l [9+3])(uint8_t *src, int topleft, int topright, int stride);
    void (*pred8x8  [4+3])(uint8_t *src, int stride);
    void (*pred16x16[4+3])(uint8_t *src, int stride);
} H264PredContext;

void ff_h264_pred_init(H264PredContext *h, int codec_id);

#endif /* FFMPEG_H264PRED_H */
