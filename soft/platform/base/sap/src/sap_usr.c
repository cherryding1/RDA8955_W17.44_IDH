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
#include "chip_id.h"

#ifndef __TARGET__
#include <malloc.h>
void pal_SimuIdleHook (void);
#else
#include "sxs_srl.h"
#include "sxs_rmt.h"
#endif  // __TARGET__

#include "sxs_lib.h"
#include "sxs_io.h"
#include "sxr_ops.h"
#include "sap_cfg.h"
#include "pal_gsm.h"
#include "sxs_spy.h"
#include "hal_sim.h"
#include "hal_map_engine.h"
#include "sxs_rmc.h"
#include "hal_debug.h"
#include "hal_signal_spy.h"

#ifdef CHIP_HAS_AP
#include "syscmds_m.h"
#endif


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
#ifndef CHIP_XTAL_CLK_IS_52M
    sxs_IoCtx.TraceBitMap [TGET_ID(_AT)] |= 1 | (1 << 2) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8)| (1 << 9) ;
    sxs_IoCtx.TraceBitMap [TGET_ID(_CSW)] |= 1 | (1 << 2) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8)| (1 << 9) ;
    //sxs_IoCtx.TraceBitMap [TGET_ID(_API)] |= 1;
    //sxs_IoCtx.TraceBitMap [TGET_ID(_MMI)] |= 1 | (1 << 2) ;
    //sxs_IoCtx.TraceBitMap [TGET_ID(_SIM)] |= 0xFFFF;
    sxs_IoCtx.TraceBitMap [TGET_ID(_MM)] |= 1 | (1 << 2) | (1 << 4)| (1 << 5)| (1 << 6) |(1<<13);
    sxs_IoCtx.TraceBitMap [TGET_ID(_RRI)] |= 1 | (1 << 2) | (1 << 4)| (1 << 7);
    sxs_IoCtx.TraceBitMap [TGET_ID(_CC)] |= 1 | (1 << 2) | (1 << 4);
    sxs_IoCtx.TraceBitMap [TGET_ID(_SS)] |= 1 | (1 << 2) | (1 << 4);
    sxs_IoCtx.TraceBitMap [TGET_ID(_SMS)] |= 1 | (1 << 2) | (1 << 4);
    sxs_IoCtx.TraceBitMap [TGET_ID(_RRD)] |= 1 | (1 << 2) | (1 << 4);
    sxs_IoCtx.TraceBitMap [TGET_ID(_L1A)] |= 1 | (1 << 1);
    sxs_IoCtx.TraceBitMap [TGET_ID(_L1S)] |= 1 | (1 << 1);
    sxs_IoCtx.TraceBitMap [TGET_ID(_SM)] |= 1 | (1 << 2) | (1 << 4);
#ifdef __DUALSIM__
    sxs_IoCtx.TraceBitMap [TGET_ID(_MSSC)] |= 1 | (1 << 2) | (1 << 3)| (1 << 4);
    sxs_IoCtx.TraceBitMap [TGET_ID(_RCO)] |= 1 | (1 << 2) | (1 << 3)| (1 << 4);
#endif //__DUALSIM__
#endif
    sxs_RmcCtxInit ();

    hal_DbgSignalSpyCfg(TRUE,TRUE,
                        HAL_DBG_SIGNAL_SPY_MODE_RF,
                        HAL_DBG_SIGNAL_SPY_IRQ_SYS_COM0,
                        HAL_DBG_SIGNAL_SPY_IRQ_SYS_FINT,
                        HAL_DBG_SIGNAL_SPY_CLK_RF);

#ifdef CHIP_HAS_AP
    syscmds_Open();
#endif

    // Print Traces for modules versions
    hal_MapEnginePrintAllModulesVersion();
}
