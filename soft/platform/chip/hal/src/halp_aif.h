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


#ifndef _HALP_AIF_H_
#define _HALP_AIF_H_

#include "cs_types.h"
#include "hal_aif.h"

// =============================================================================
//  MACROS
// =============================================================================
/// Indices for the user handler array.
#define RECORD  0
#define PLAY    1

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_AIF_XFER_HANDLER_T
// -----------------------------------------------------------------------------
/// Type privately used to store both handler for each direction
// =============================================================================
typedef struct
{
    HAL_AIF_HANDLER_T halfHandler;
    HAL_AIF_HANDLER_T endHandler;
} HAL_AIF_XFER_HANDLER_T;






// =============================================================================
// hal_AifIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when a BB-IFC interrupt occurs.
///
// =============================================================================
PROTECTED VOID hal_AifIrqHandler(UINT8 interruptId);


#endif //_HALP_AIF_H_

