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

#include "cs_types.h"
#include "chip_id.h"

#include "snd_types.h"
#include "calib_m.h"

#include "string.h"



PUBLIC VOID calib_ConvertStructV2p5(CALIB_BB_T *pCalibBb)
{
    for (int i = 0; i < CALIB_AUDIO_ITF_QTY; i++)
    {
        memcpy(&pCalibBb->audio[i].audioGains.inGainsRecord,
               &pCalibBb->audio[i].audioGains.inGainsCall,
               sizeof(pCalibBb->audio[i].audioGains.inGainsRecord));
    }
}


