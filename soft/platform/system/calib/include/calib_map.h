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


#ifndef _CALIB_MAP_H_
#define _CALIB_MAP_H_


#include "calib_m.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CALIB_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// This struct will contain pointers to the calibration info and to the struct where
/// to put the calibration context requests. It also contains the address of the
/// calibration sector in flash.
// =============================================================================
typedef CALIB_CALIBRATION_T CALIB_MAP_ACCESS_T;





// =============================================================================
// calib_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID calib_RegisterYourself(VOID);



#endif

