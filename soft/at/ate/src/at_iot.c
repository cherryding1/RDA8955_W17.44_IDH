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

#ifdef IOT_SUPPORT
#include "at.h"
#include "at_sa.h"

#define IOT_TASK_STACK_SIZE    (2048 * 2)
#define IOT_TASK_PRIORITY      COS_MMI_TASKS_PRIORITY_BASE

VOID IOT_Task(VOID *pData);
HANDLE g_IOT_Task = HNULL;

BOOL IOT_Init(VOID)
{
    g_IOT_Task = COS_CreateTask(IOT_Task,
                                NULL, NULL, IOT_TASK_STACK_SIZE, IOT_TASK_PRIORITY, 0, 0, "BAL_MmiTask");
}

VOID IOT_Handler_Raw_Event(COS_EVENT *event)
{
    //handle raw event, then send to IOT_Event_Handler
    //IOT_Event_Handler(id, msg);
}

VOID IOT_Task(VOID *pData)
{
    COS_EVENT event = { 0 };

    AT_TC(g_sw_SA, "Enter IOT_Task()...");

    for (;;)
    {
        COS_WaitEvent(g_IOT_Task, &event, COS_WAIT_FOREVER);
        IOT_Handler_Raw_Event(&event);
    }
}

#endif
