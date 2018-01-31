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








#include "sxs_type.h"
#include "sxs_srl.h"
#include "sxs_rmt.h"
#include "sxs_lib.h"
#include "sxs_io.h"
#include "sxr_ops.h"
#include "sxs_spy.h"
#include "sxs_rmc.h"
#include "hal_debug.h"
#define __SAP_CFG_VAR__
#include "sap_cfg.h"
#undef __SAP_CFG_VAR__
#include "stt.h"

#include "reftestp_debug.h"

#include "hal_map_engine.h"
//#include "uctls_m.h"
#include "pmd_m.h"


// @todo Put this in a dedicated file.
PRIVATE VOID reftest_ChargerHandler(PMD_CHARGER_STATUS_T status)
{
    // Just display the new charger status in the trace.
    switch (status)
    {
    case PMD_CHARGER_UNPLUGGED:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: PMD_CHARGER_UNPLUGGED");
        break;

    case PMD_CHARGER_PLUGGED:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: PMD_CHARGER_PLUGGED");
        break;

    case PMD_CHARGER_FAST_CHARGE:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: PMD_CHARGER_FAST_CHARGE");
        break;

    case PMD_CHARGER_PULSED_CHARGE:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: PMD_CHARGER_PULSED_CHARGE");
        break;

    case PMD_CHARGER_FULL_CHARGE:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: PMD_CHARGER_FULL_CHARGE");
        break;

    default:
        REFTEST_TRACE(TSTDOUT, 0, "Charger new status: %d", status);
        break;
    }
}


/*
==============================================================================
   Function   : sap_InitUsr
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/

void sap_InitUsr(void)
{
    //------------------------
    // Create OS Malloc clusters
    //------------------------
    sxr_NewCluster (60, 175);
    sxr_NewCluster (220, 255);
    // sxr_NewCluster (550, 22);
    sxr_NewCluster (640, 90);
    sxr_NewCluster (1600, 8);
    // sxr_NewCluster (6000, 3);
    sxr_NewCluster (7000, 8);

    // -----------------------------
    // Setup default trace levels
    // -----------------------------
    sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] = 0xffff;

    // Create STT task
    //sxs_CmnTaskId [CMN_PAL] = sxr_NewTask (&stt_TaskDesc);
    //sxr_StartTask (sxs_CmnTaskId [CMN_PAL], &SttParam);

    // Create User Task
    REFTEST_TRACE(TSTDOUT, 0, "creating test task");
    testTaskId = sxr_NewTask(&sxs_testTaskDesc);
    sxr_StartTask(testTaskId, NULL);
    testMbx = sxr_NewMailBox();

    // Initiate charger status
    reftest_ChargerHandler(pmd_GetChargerStatus());
    // Configure PMD to warn the reftest when a charger is plugged.
    pmd_SetChargerStatusHandler(reftest_ChargerHandler);

    // Print Traces for HAL and calibration
    // versions, which cannot be printed at registration
    // as the trace is not working at the time they
    // are recorded.
    hal_MapEnginePrintAllModulesVersion();
}

