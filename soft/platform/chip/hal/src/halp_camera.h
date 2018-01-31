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


#ifndef _HALP_CAMERA_H_
#define _HALP_CAMERA_H_

#include "hal_sys.h"
// =============================================================================
// hal_CameraIrqHandler
// -----------------------------------------------------------------------------
/// Handler called by the IRQ module when the camera module generates an
/// interrupt.
// =============================================================================
PROTECTED VOID hal_CameraIrqHandler(UINT8 interruptId);

#endif // _HALP_CAMERA_H_


