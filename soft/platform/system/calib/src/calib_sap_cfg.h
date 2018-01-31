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
EXPORT UINT8 g_calibStubTaskId;
EXPORT UINT8 g_calibStubMbx;

EXPORT VOID calib_StubTaskInit (VOID);
EXPORT VOID calib_SxsTaskIdle (VOID);
EXPORT VOID calib_SapInitUsr(VOID);

#define CALIB_CALIB_TASK_STACK_SIZE   4*128
#define CALIB_CALIB_TASK_PRIORITY     200


#define CALIB_IDLE_TASK_STACK_SIZE 2*128
#define CALIB_IDLE_TASK_PRIORITY   255

#ifdef __SAP_CFG_VAR__
CONST sxr_TaskDesc_t g_calibSxsTaskIdleDesc =
{
    (VOID(*)(VOID *))calib_SxsTaskIdle,
    0,
    "Calibration Idle Task.",
    CALIB_IDLE_TASK_STACK_SIZE,
    CALIB_IDLE_TASK_PRIORITY
};

CONST sxr_TaskDesc_t g_calibSxsCalibStubTaskDesc =
{
    (VOID(*)(VOID *))calib_StubTaskInit,
    NIL,
    "Calibration Calib. Task",
    CALIB_CALIB_TASK_STACK_SIZE,
    CALIB_CALIB_TASK_PRIORITY
};

#else

EXPORT CONST sxr_TaskDesc_t g_calibSxsTaskIdleDesc;
EXPORT CONST sxr_TaskDesc_t g_calibSxsCalibTaskDesc;


#endif

#endif


