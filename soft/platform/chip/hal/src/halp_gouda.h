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


#if CHIP_HAS_GOUDA

#ifndef _HALP_GOUDA_H_
#define _HALP_GOUDA_H_

#include "hal_sys.h"

// =============================================================================
// hal_GoudaIrqHandler
// -----------------------------------------------------------------------------
// =============================================================================
PROTECTED VOID hal_GoudaIrqHandler(UINT8 interruptId);


#endif // _HALP_GOUDA_H_

#endif // CHIP_HAS_GOUDA

