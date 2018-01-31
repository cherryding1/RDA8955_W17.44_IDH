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




#ifndef _MBPREDICTION_H_
#define _MBPREDICTION_H_

#include "portab.h"
#include "decoder.h"
#include "mpeg4_global.h"

//#define MIN(X, Y) ((X)<(Y)?(X):(Y))
//#define MAX(X, Y) ((X)>(Y)?(X):(Y))

VECTOR
get_pmv2(uint16_t mb_width,VECTOR *pMVBuffPred[3],const int block);

#endif                          /* _MBPREDICTION_H_ */

