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

#ifndef MMC_TIMER_H
#define MMC_TIMER_H

#include "cos.h"

typedef enum
{
    /*
    ** Start for for mmc timer.
    ** Total 256 IDS.
    */
    MMC_TIMER_ID_NONE=COS_MDI_TIMER_ID_BASE, /* Imp: Please do not modify this */

    MMC_VID_PLY_TIMER_ID,
    MMC_VID_REC_TIMER_ID,
    MMC_CAMERA_TIMER_ID,
    MMC_AUD_PLY_TIMER_ID,
    MMC_AUD_REC_TIMER_ID,

    MMC_MAX_TIMERS=COS_MDI_TIMER_ID_END_ /* Imp: Please do not modify this */
} MMC_TIMER_IDS;

#endif


