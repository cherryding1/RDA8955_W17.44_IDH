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

#ifndef __HOSTGSM
#define __HOSTGSM

#include <stdio.h>
#include "vpp_hr_typedefs.h"
#include "hal_speech.h"
/*_________________________________________________________________________
 |                                                                         |
 |                           Function Prototypes                           |
 |_________________________________________________________________________|
*/
//void dePackEnc(UINT16* In, UINT16* Out);

void   fillBitAlloc(int iVoicing, int iR0, int *piVqIndeces,
                    int iSoftInterp, int *piLags,
                    int *piCodeWrdsA, int *piCodeWrdsB,
                    int *piGsp0s, INT16 swVadFlag,
                    INT16 swSP, INT16 *pswBAlloc);


#endif
