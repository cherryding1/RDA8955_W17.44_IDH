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
#include "spalp_private.h"
#include "spal_debug.h"
#include "spal_cfg.h"
#include "global_macros.h"


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

// =============================================================================
// spal_Open
// -----------------------------------------------------------------------------
/// This function MUST be in RAM and not in ROM !
/// This function initializes the global parameter structure used by SPAL.
/// @param pCfg SPAL_GLOBAL_PARAMS_T. Pointer to the configuration structure.
// =============================================================================
PUBLIC VOID spal_Open (SPAL_GLOBAL_PARAMS_T* pCfg)
{
    // Initialized GDB variables.
    spal_GdbOpen();

    spal_IrqInit(pCfg->irqInRom);
    g_spalIrqRegistry = pCfg->irqHandler;
    spal_IrqSetMask();
}

