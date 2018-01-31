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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif
#ifdef MP4_3GP_SUPPORT

#ifndef _MBPREDICTION_H_
#define _MBPREDICTION_H_

#include "portab.h"
#include "decoder.h"
#include "mpeg4_global.h"

#define RDIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define RSHIFT(a,b) ((a) > 0 ? ((a) + ((1<<(b))>>1))>>(b) : ((a) + ((1<<(b))>>1)-1)>>(b))
#define MLT(i)  (((16-(i))<<16) + (i))
static const uint32_t MTab[16] =
{
    MLT( 0), MLT( 1), MLT( 2), MLT( 3), MLT( 4), MLT( 5), MLT( 6), MLT( 7),
    MLT( 8), MLT( 9), MLT(10), MLT(11), MLT(12), MLT(13), MLT(14), MLT(15)
};
#undef MLT

//#define MIN(X, Y) ((X)<(Y)?(X):(Y))
//#define MAX(X, Y) ((X)>(Y)?(X):(Y))

VECTOR
get_pmv2(uint16_t mb_width,VECTOR *pMVBuffPred[3],const int block);

#endif                          /* _MBPREDICTION_H_ */
#endif

