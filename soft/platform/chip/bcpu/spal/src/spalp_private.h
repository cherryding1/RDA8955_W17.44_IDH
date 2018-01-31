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



#ifndef SPALP_PRIVATE_H
#define SPALP_PRIVATE_H


//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------

#include "cs_types.h"
// spal public headers
#include "spal_vitac.h"
#include "spal_cap.h"
#include "spal_irq.h"


//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------


// Number/size of Tsc Cross-correlation for time syncronization
#define NB_TSC_XCOR_SIZE 15
#define SCH_TSC_XCOR_SIZE 63


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

EXPORT PROTECTED SPAL_IRQ_HANDLER_T g_spalIrqRegistry;


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//======================================================================
// spal_InvalidateDcache
//----------------------------------------------------------------------
/// Data cache invalidation
/// !!! THIS FUNCTION SHOULD BE COMPILED IN MIPS32 !!!
///
/// @addtogroup misc_func
//======================================================================
PROTECTED VOID spal_InvalidateDcache(VOID);


//======================================================================
// spal_InvalidateCache
//----------------------------------------------------------------------
// Both caches invalidation
// !!! THIS FUNCTION SHOULD BE COMPILED IN MIPS32 !!!
//======================================================================
PROTECTED VOID spal_InvalidateCache(VOID);


//======================================================================
// spal_IrqInit
//======================================================================
/// Select the handler to use for the bcpu
/// @param irqInRam If \c TRUE, the IRQ handler in internal ROM is used.
/// If \c FALSE, the one in internal SRAM is used.
//======================================================================
PROTECTED VOID spal_IrqInit(BOOL irqInRom);


//======================================================================
// spal_SetIrqMask
//----------------------------------------------------------------------
// Setup the IRQ mask settings for BaseBand CPU
//
// @todo see if we want to give more control to SPC
//======================================================================
PROTECTED VOID spal_IrqSetMask(VOID);


// ============================================================================
// spal_IrqHandler
// ----------------------------------------------------------------------------
/// spal IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by g_spalIrqRegistry.
// ============================================================================
PROTECTED VOID spal_IrqHandler(VOID);


#endif // SPALP_PRIVATE_H
