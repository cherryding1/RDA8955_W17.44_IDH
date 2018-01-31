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
#include "cpu_share.h"
#include "mcip_debug.h"

#include "mmc.h"

//#include "cpu_sleep.h"
#include "lily_statemachine.h"

#include "mmc_fmrecorder.h"
uint32 LILY_FmrRecordStart (int8 *file_name_p)  //  MCI_MEDIA_PLAY_REQ,
{
    int32 result;
    //int32 recordtype;

    diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordStart\n");
    if (STA_RES_FAIL == lily_changeToState(STM_MOD_FMREC))
    {
        diag_printf("Error state!!!!!!!!!!!in bmpshow\n");
        return -1;
    }

    mmc_SetCurrMode(MMC_MODE_FM_RECORD);
    result = FmrRecordStart(0, 0, file_name_p);
    mmc_SetModeStatus(AudioRecordStatus);

    if (result > 0)
    {
        mmc_SetCurrMode(MMC_MODE_IDLE);
    }
    return result;
}

uint32 LILY_FmrRecordPause(void)
{
    diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordPause\n");

    FmrRecordPause();
    mmc_SetModeStatus(AudioRecordPauseStatus);
    return 0;
    //mmc_SendResult(nEvent,1);
}

uint32 LILY_FmrRecordResume(void)
{
    diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordResume\n");

    FmrRecordResume();
    mmc_SetModeStatus(AudioRecordStatus);
    return 0;
//  mmc_SendResult(nEvent,1);

}

uint32 LILY_FmrRecordStop(void)
{
    diag_printf("[FMR_RECORD_API]: CALL - LILY_FmrRecordStop\n");

    FmrRecordStop();
    mmc_SetModeStatus(AudioRecordStopStatus);
    mmc_SetCurrMode(MMC_MODE_IDLE);
    lily_exitModule(STM_MOD_FMREC);
    return 0;
    //mmc_SendResult(nEvent,1);
}


