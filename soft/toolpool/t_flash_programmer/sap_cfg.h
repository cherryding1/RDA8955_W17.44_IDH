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
#include "stt.h"

/******************************************************************************
       OS TASKS CONFIGURATION
******************************************************************************/
extern UINT8 testTaskId;
extern UINT8 testMbx;

extern void testTaskInit (void);
extern void sxs_TaskIdle (void);
//extern stt_Param_t SttParam;
//extern void stt_Task (stt_Param_t * Param);

#define STT_TASK_STACK_SIZE  300
#define STT_TASK_PRIORITY    0

#define TEST_TASK_STACK_SIZE   (4*1024)
#define TEST_TASK_PRIORITY     200


#define IDLE_TASK_STACK_SIZE (4*1024)
#define IDLE_TASK_PRIORITY   255

#ifdef __SAP_CFG_VAR__
const sxr_TaskDesc_t sxs_TaskIdleDesc =
{
    (void(*)(void *))sxs_TaskIdle,
    0,
    "Task Idle.",
    IDLE_TASK_STACK_SIZE,
    IDLE_TASK_PRIORITY
};

const sxr_TaskDesc_t sxs_testTaskDesc =
{
    (void(*)(void *))testTaskInit,
    NIL,
    "Test Task",
    TEST_TASK_STACK_SIZE,
    TEST_TASK_PRIORITY
};



#if 0
const sxr_TaskDesc_t stt_TaskDesc =
{
    (void(*)(void *))stt_Task,
    0,
    "STT Task.",
    STT_TASK_STACK_SIZE,
    STT_TASK_PRIORITY
};
#endif
#else
extern const sxr_TaskDesc_t sxs_TaskIdleDesc;
extern const sxr_TaskDesc_t sxs_testTaskDesc;
extern const sxr_TaskDesc_t stt_TaskDesc;
#endif


#endif
