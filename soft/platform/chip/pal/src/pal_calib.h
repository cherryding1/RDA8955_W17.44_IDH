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

















#ifndef _PAL_CALIB_H_
#define _PAL_CALIB_H_

#include "calib_m.h"


/* -------------------------------------------------- */
/* ----- Window timing parameter accessors. */

#define PAL_TIME(n)                 (g_palCalibration->bb->times[(n)])

/* Tx Up */
#define PAL_TIME_GSMK_UP            0
#define PAL_TIME_DAC_UP             1
#define PAL_TIME_TX_OEN_UP          2

/* Tx Down */
#define PAL_TIME_GSMK_DN            3
#define PAL_TIME_DAC_DN             4
#define PAL_TIME_TX_OEN_DN          5
#define PAL_TIME_DTX_OFF_DN         6

/* Rx Up */
#define PAL_TIME_RF_IN_UP           7
#define PAL_TIME_ADC_UP             8
#define PAL_TIME_FIRST_GAIN         9
#define PAL_TIME_NEXT_GAIN          10

/* Rx Down */
#define PAL_TIME_RF_IN_DN           11
#define PAL_TIME_ADC_DN             12





#endif /* _PAL_CALIB_H_ */

