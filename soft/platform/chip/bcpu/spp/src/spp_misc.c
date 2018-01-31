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



#include "cs_types.h"
#include "global_macros.h"
#include "spal_irq.h"
#include "spal_mem.h"


//------------------------------------------------------------------------------
// globals (must be at a fixed place in SRAM)
//------------------------------------------------------------------------------
volatile UINT8 g_sppFullBurstReady SPAL_BBSRAM_GLOBAL = 0;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// Functions used for EQU_HBURST_MODE mechanism
PUBLIC UINT8 spp_FullBurstReady(VOID)
{
    return g_sppFullBurstReady;
}

PUBLIC VOID spp_ResetFullBurstReady(VOID)
{
    g_sppFullBurstReady = 0;
}

PUBLIC VOID spp_OneFullBurstReady(VOID)
{
    volatile UINT32 critical_sec = spal_IrqEnterCriticalSection();
    g_sppFullBurstReady++;
    spal_IrqExitCriticalSection(critical_sec);
}

PUBLIC VOID spp_OneFullBurstDone(VOID)
{
    volatile UINT32 critical_sec = spal_IrqEnterCriticalSection();
    g_sppFullBurstReady--;
    spal_IrqExitCriticalSection(critical_sec);
}



