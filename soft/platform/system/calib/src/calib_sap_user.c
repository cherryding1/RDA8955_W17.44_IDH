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
#include "calib_sap_cfg.h"
#undef __SAP_CFG_VAR__

/*
==============================================================================
   Function   : calib_SapInitUsr
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : None.
   Return     : None.
==============================================================================
*/

VOID calib_SapInitUsr(VOID)
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
    sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] |= 0xFFFF;


    // Create User Task
    SXS_TRACE(TSTDOUT,"creating test task ");
    g_calibStubTaskId = sxr_NewTask(&g_calibSxsCalibStubTaskDesc);
    sxr_StartTask(g_calibStubTaskId,NULL);
    g_calibStubMbx = sxr_NewMailBox();
}

