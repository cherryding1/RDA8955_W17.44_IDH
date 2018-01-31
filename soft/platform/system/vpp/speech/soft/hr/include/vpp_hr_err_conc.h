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

#ifndef __ERR_CONC
#define __ERR_CONC

#include "vpp_hr_typedefs.h"

/*_________________________________________________________________________
 |                                                                         |
 |                            Function Prototypes                          |
 |_________________________________________________________________________|
*/

void   para_conceal_speech_decoder(INT16 pswErrorFlag[],
                                   INT16 pswSpeechPara[], INT16 *pswMutePermit);

INT16 level_calc(INT16 swInd, INT32 *pl_en);

void   level_estimator(INT16 update, INT16 *pswLevelMean,
                       INT16 *pswLevelMax,
                       INT16 pswDecodedSpeechFrame[]);

void   signal_conceal_sub(INT16 pswPPFExcit[],
                          INT16 ppswSynthAs[], INT16 pswSynthFiltState[],
                          INT16 pswLtpStateOut[], INT16 pswPPreState[],
                          INT16 swLevelMean, INT16 swLevelMax,
                          INT16 swErrorFlag1, INT16 swMuteFlagOld,
                          INT16 *pswMuteFlag, INT16 swMutePermit);

#endif
