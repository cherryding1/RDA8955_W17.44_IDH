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





#ifndef  _HALP_PAGE_SPY_H_
#define  _HALP_PAGE_SPY_H_

#include "cs_types.h"
#include "hal_debug.h"


//Types and enums


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// hal_DbgPageProtectGetHitMaster
// -----------------------------------------------------------------------------
/// Get the master that hit the page
/// @param page One of the four protected page
/// @return Master id of the one that hit the page
// =============================================================================
PROTECTED UINT8 hal_DbgPageProtectGetHitMaster(HAL_DBG_PAGE_NUM_T page);




// =============================================================================
// hal_DbgPageProtectGetHitAddress
// -----------------------------------------------------------------------------
/// Get address where \c page where hit
/// @param page Hit page whose address where it was hit is requested
/// @return The last address hit in a protected page
// =============================================================================
PROTECTED UINT32 hal_DbgPageProtectGetHitAddress(HAL_DBG_PAGE_NUM_T page);



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
PROTECTED HAL_DBG_PAGE_SPY_MODE_T hal_DbgPageProtectGetHitMode(HAL_DBG_PAGE_NUM_T page);


#endif // _HALP_PAGE_SPY_H_
