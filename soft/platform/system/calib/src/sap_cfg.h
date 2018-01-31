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





#ifndef __SAP_CFG_H__
#define __SAP_CFG_H__

#include "sxs_type.h"
#include "sxs_cfg.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxs_lib.h"
#include "cmn_defs.h"

/******************************************************************************
       OS TASKS CONFIGURATION
******************************************************************************/

#define IDLE_TASK_STACK_SIZE 2*128
#define IDLE_TASK_PRIORITY   255

extern void sxs_TaskIdle (void);


const sxs_TaskCfg_t *sxs_TaskCfg = 0;

const sxr_TaskDesc_t sxs_TaskIdleDesc =
{
    (void(*)(void *))sxs_TaskIdle,
    0,
    "Task Idle.",
    IDLE_TASK_STACK_SIZE,
    IDLE_TASK_PRIORITY
};


const u8 sxs_NbStaticTasksDesc = 0;

#endif
