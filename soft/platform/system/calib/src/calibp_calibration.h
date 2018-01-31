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


#ifndef _CALIBP_CALIBRATION_H_
#define _CALIBP_CALIBRATION_H_

#include "cs_types.h"
#include "calib_m.h"

// =============================================================================
// g_calibCalibration
// -----------------------------------------------------------------------------
/// Calibration Structure.
/// As we are in the calibration module, the structure can be directly accessed,
/// without using a pointer, as it is declared in this module.
// =============================================================================
EXPORT PROTECTED CALIB_CALIBRATION_T g_calibCalibration;



// =============================================================================
// calib_SendCalibDoneMsg
// -----------------------------------------------------------------------------
/// Send calib done message to AP.
// =============================================================================
PROTECTED VOID calib_SendCalibDoneMsg(VOID);


#endif // _CALIBP_CALIBRATION_H_



