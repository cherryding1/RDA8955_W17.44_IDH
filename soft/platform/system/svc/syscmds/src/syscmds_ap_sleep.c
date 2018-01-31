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


#include "syscmdsp_debug.h"

#include "hal_ap_comm.h"
#include "sxr_tksd.h"
#include "sxr_sbx.h"
#include "string.h"


PRIVATE VOID ap_SleepTask(VOID *param);


// =============================================================================
// MACROS
// =============================================================================

// In number of words
#define AP_SLEEP_TASK_STACK_SIZE    (256)

#define AP_SLEEP_TASK_PRIORITY      (204)

#define AP_SLEEP_FLAG               (0x32abc892)

#define AP_WAKEUP_FLAG              (0x89325bca)


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE CONST sxr_TaskDesc_t gc_apSleepTaskDesc =
{
    .TaskBody = &ap_SleepTask,
    .TaskExit = NULL,
    .Name = "AP Sleep Task",
    .StackSize = AP_SLEEP_TASK_STACK_SIZE,
    .Priority = AP_SLEEP_TASK_PRIORITY,
};

#if defined(MODEM_RUN_IN_DDR) && defined(DYNAMAIC_TURN_DDR)
PRIVATE DDR_SLEEP_DATA UINT32 g_apSleepTaskStack[AP_SLEEP_TASK_STACK_SIZE];
#endif


PRIVATE UINT8 g_apSleepMbx;
PRIVATE UINT8 g_apSleepTaskId;


// =============================================================================
//  FUNCTIONS
// =============================================================================

PRIVATE VOID ap_SleepHandler(UINT32 sleep)
{
    UINT32 evt[SXR_EVT_MBX_SIZE];
    evt[0] = sleep ? AP_SLEEP_FLAG : AP_WAKEUP_FLAG;

    sxr_Send(evt, g_apSleepMbx, SXR_SEND_EVT);
}

PRIVATE VOID ap_SleepTask(VOID *param)
{
    UINT32 evt[SXR_EVT_MBX_SIZE];
    VOID *msg;

    while (1)
    {
        msg = sxr_Wait(evt, g_apSleepMbx);
        SYSCMDS_ASSERT(msg == NIL, "Events are expected instead of msgs");

        if (evt[0] == AP_SLEEP_FLAG)
        {
            hal_ApCommApSleepProc();
        }
        else if (evt[0] == AP_WAKEUP_FLAG)
        {
            hal_ApCommApWakeupProc();
        }
        else
        {
            SYSCMDS_ASSERT(FALSE, "Invalid event received: 0x%08x", evt[0]);
        }
    }
}

PUBLIC VOID ap_SleepInit(VOID)
{
    g_apSleepMbx = sxr_NewMailBox();

#if defined(MODEM_RUN_IN_DDR) && defined(DYNAMAIC_TURN_DDR)
    g_apSleepTaskId = sxr_NewTaskExt(&gc_apSleepTaskDesc, g_apSleepTaskStack);
#else
    g_apSleepTaskId = sxr_NewTask(&gc_apSleepTaskDesc);
#endif

    sxr_StartTask(g_apSleepTaskId, NULL);

    hal_ApCommSetApSleepCallback(&ap_SleepHandler);
}

