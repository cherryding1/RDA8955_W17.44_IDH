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

#include <cswtype.h>
#include <base.h>
#include "chip_id.h"
#include "hal_timers.h"
#include <cos.h>
#include <base_prv.h>
#include <dm_prv.h>

#define DM_DEVID_KEYPAD 0
#define DM_DEVID_ALRAM  1
#define DM_DEVID_XX

extern BOOL COS_SendSysEvent(
    COS_EVENT *pEvent,
    UINT16 nOption
);
extern VOID DM_AudInit(VOID);

BOOL g_KeyBoard_Init_Status = FALSE ;

BOOL DM_KeyboardInit()
{
    if (g_KeyBoard_Init_Status)
    {
        return TRUE ;
    }

    g_KeyBoard_Init_Status = TRUE ;
    // Key Init
    DM_KeyInit();
    return TRUE ;
}

PRIVATE VOID alarm_isr(VOID)
{
    COS_EVENT ev;
    ev.nEventId = EV_TIM_ALRAM_IND;
    ev.nParam1  = 0;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendSysEvent(&ev, (UINT16)0);
}

BOOL DM_Init()
{
    // Alarm Init
    hal_TimRtcIrqSetMask(TRUE);
    hal_TimRtcAlarmIrqSetHandler(alarm_isr);
    // Audio Init
    DM_AudInit();
    // Keyboard Init
    DM_KeyboardInit();
    return TRUE;
}

BOOL DM_SendToDefaultMMITask(COS_EVENT *pev)
{
    return COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), pev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

