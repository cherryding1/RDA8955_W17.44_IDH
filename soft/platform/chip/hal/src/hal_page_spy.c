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
#include "page_spy.h"

#include "halp_page_spy.h"
#include "halp_debug.h"

#include "hal_debug.h"
#include "hal_sys.h"


// =============================================================================
// hal_PageProtectSetup
// -----------------------------------------------------------------------------
/// This function setups a protection page
///
/// @param pageNum Name of the page we want to configure (#HAL_DBG_PAGE_UD0)
/// @param mode Protection mode
/// @param startAddr Address of the beginning of the page
/// @param endAddr  End address of the page. This address is not included in
/// the page
// =============================================================================
PUBLIC VOID hal_DbgPageProtectSetup(
    HAL_DBG_PAGE_NUM_T pageNum, HAL_DBG_PAGE_SPY_MODE_T mode,
    UINT32 startAddr, UINT32 endAddr)
{
    // FIXME Define special function for hal_init and set the asserts for
    // the user function !
//    HAL_ASSERT(pageNum<2,"Page number must be HAL_PAGE_UD0 (page number tried: %d", pageNum);
//    HAL_ASSERT(hwp_pageSpy->enable & (1<<pageNum),"Page %d already enabled", pageNum);
    UINT32 status = hal_SysEnterCriticalSection();
    hwp_pageSpy->page[pageNum].start = ((startAddr) & (~PAGE_SPY_MODE(3))) | PAGE_SPY_MODE(mode);
    hwp_pageSpy->page[pageNum].end = endAddr;
    hal_SysExitCriticalSection(status);
}



// =============================================================================
// hal_DbgPageProtectEnable
// -----------------------------------------------------------------------------
/// Enable the protection of a given page.
///
/// In case of a user defined page, don't forget to configure it with
/// #hal_DbgPageProtectSetup before.
///
/// #HAL_DBG_PAGE_IRQ_STACKWATCH, #HAL_DBG_PAGE_CODEWATCH and
/// #HAL_DBG_PAGE_INT_CODEWATCH are already
/// enabled by default during the boot.
///
/// @param pageNum Page to enable
// =============================================================================
PUBLIC VOID hal_DbgPageProtectEnable(HAL_DBG_PAGE_NUM_T pageNum)
{
    HAL_ASSERT(pageNum<HAL_DBG_PAGE_QTY,"Page number must be HAL_PAGE_UD0 (page number tried: %d", pageNum);

    hwp_pageSpy->enable = 1<<pageNum; //set register
}



// =============================================================================
// hal_DbgPageProtectDisable
// -----------------------------------------------------------------------------
/// Disable the protection of a given page
/// You shouldn't want to disable the protection on
/// #HAL_DBG_PAGE_IRQ_STACKWATCH,
/// #HAL_DBG_PAGE_CODEWATCH and #HAL_DBG_PAGE_INT_CODEWATCH.
///
/// @param pageNum Page to disable
// =============================================================================
PUBLIC VOID hal_DbgPageProtectDisable(HAL_DBG_PAGE_NUM_T pageNum)
{
    HAL_ASSERT(pageNum<HAL_DBG_PAGE_QTY,"Page number must be HAL_PAGE_UD0 (page number tried: %d", pageNum);

    hwp_pageSpy->disable = (1<<pageNum);
}



// =============================================================================
// hal_DbgPageProtectGetHitMaster
// -----------------------------------------------------------------------------
/// Get the master that hit the page
/// @param page One of the four protected page
/// @return Master id of the one that hit the page
// =============================================================================
PROTECTED UINT8 hal_DbgPageProtectGetHitMaster(HAL_DBG_PAGE_NUM_T page)
{
    return  hwp_pageSpy->page[page].master;
}




// =============================================================================
// hal_DbgPageProtectGetHitAddress
// -----------------------------------------------------------------------------
/// Get address where \c page where hit
/// @param page Hit page whose address where it was hit is requested
/// @return The last address hit in a protected page
// =============================================================================
PROTECTED UINT32 hal_DbgPageProtectGetHitAddress(HAL_DBG_PAGE_NUM_T page)
{
    return  hwp_pageSpy->page[page].addr & (~PAGE_SPY_MODE(3));
}




// =============================================================================
// hal_DbgPageProtectGetHitMode
// -----------------------------------------------------------------------------
/// Get the mode that triggered the page
///
/// @param page A page that triggered the Page Spy
/// @return The access mode that triggered the Page Spy : \n
///     - If the bit 0 is set, a read access  happened
///     - If the bit 1 is set, a write access happened
///     .
// =============================================================================
PROTECTED HAL_DBG_PAGE_SPY_MODE_T hal_DbgPageProtectGetHitMode(HAL_DBG_PAGE_NUM_T page)
{
    return     ((hwp_pageSpy->page[page].addr)>>30) & 3;
}

PUBLIC BOOL hal_DbgPageProtectIsEnabled(HAL_DBG_PAGE_NUM_T pageNum)
{
    UINT32 status;
    HAL_ASSERT(pageNum<HAL_DBG_PAGE_QTY,"Page number must be HAL_PAGE_UD0 (page number tried: %d", pageNum);
    status = hal_SysEnterCriticalSection();

    if (hwp_pageSpy->enable & 1<<pageNum)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
    hal_SysExitCriticalSection(status);
}


