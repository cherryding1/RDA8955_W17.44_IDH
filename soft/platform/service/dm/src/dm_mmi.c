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































#include "chip_id.h"
#include <errorcode.h>
#include <cos.h>
#include <base_prv.h>
#include <dm.h>
#include <drv.h>

#include "hal_pwm.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_lps.h"
#include "dbg.h"

#include "csw_csp.h"



// =============================================================================
// csw_SetResourceActivity
// -----------------------------------------------------------------------------
/// Require a minimum system frequency for a given resource.
/// To release the resource, allowing for lowpower, you have to require a
/// frequenct of 32kHz, ie: #CSW_SYS_FREQ_32K.
/// Any other requirement will take the resource.
///
/// @param resource Resource that requests a minimum system frequency.
/// @param freq Minimum system frequency required. The actual system fequency
/// may be faster.
// =============================================================================
PUBLIC VOID csw_SetResourceActivity(CSW_LP_RESOURCE_T resource, CSW_SYS_FREQ_T freq)
{
    // Modem2G class or above
    DBG_ASSERT((HAL_SYS_FREQ_APP_USER_0 + resource) <= HAL_SYS_FREQ_APP_LAST_USER, "Illegal resource number %d", resource);

    if (freq != CSW_SYS_FREQ_32K)
        hal_SwRequestClk(FOURCC_CSW0 + resource, (HAL_SYS_FREQ_T)freq);
    else
        hal_SwReleaseClk(FOURCC_CSW0 + resource);
}

PUBLIC CSW_SYS_FREQ_T csw_GetResourceActivity(CSW_LP_RESOURCE_T resource)
{
    UINT32 freq;
    // Modem2G class or above
    DBG_ASSERT((HAL_SYS_FREQ_APP_USER_0 + resource) <= HAL_SYS_FREQ_APP_LAST_USER, "Illegal resource number %d", resource);
    freq = hal_SwGetClkRate(FOURCC_CSW0 + resource);
    return (CSW_SYS_FREQ_T) freq;
}

PUBLIC VOID csw_SetVOCFreqActivity(CSW_VOC_FREQ_T freq)
{
    // Modem2G class or above
    hal_SysSetVocClock((HAL_SYS_VOC_FREQ_T)freq);
}


UINT8 uLcdLevel = 0xff;
UINT8 uKeyLevel = 0xff;
BOOL bSetClkCalTimer  = FALSE;



VOID DM_KeyBackLightOpen()
{
    DRV_SetKeyBacklight(uKeyLevel);
}

VOID DM_KeyBackLightClose()
{
    DRV_SetKeyBacklight(0);
}

VOID DM_LpwMmiActive()
{
    if(bSetClkCalTimer)
    {
        COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_DM_LPCLKCALIB_TIMER_ID);
        bSetClkCalTimer = FALSE;
    }
}

VOID DM_LpwMmiInactive()
{
    if(bSetClkCalTimer)
    {
        COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_DM_LPCLKCALIB_TIMER_ID);
        bSetClkCalTimer = FALSE;
    }
    if(COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), PRV_DM_LPCLKCALIB_TIMER_ID, COS_TIMER_MODE_SINGLE, MMI_LPCLKCALIB_TIMER))
    {
        bSetClkCalTimer = TRUE;
    }
}

