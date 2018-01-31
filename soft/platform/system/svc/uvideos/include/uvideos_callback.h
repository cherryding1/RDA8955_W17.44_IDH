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

#ifndef _UVIDEOS_CALLBACK_H_
#define _UVIDEOS_CALLBACK_H_

#include "cs_types.h"

#include "uctls_callback.h"

/// @file uvideos_callback.h
///
/// @addtogroup uvideos
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
EXPORT PUBLIC CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T    g_uvideosIad;


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// g_uvideosCallback
// -----------------------------------------------------------------------------
/// Mass storage service callback
// =============================================================================
EXPORT PUBLIC CONST UCTLS_SERVICE_CALLBACK_T    g_uvideosCallback;

/// @} <- End of the uvideos group

#endif // _UVIDEOS_CALLBACK_H_

