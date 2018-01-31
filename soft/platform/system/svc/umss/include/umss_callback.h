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


#ifndef _UMSS_CALLBACK_H_
#define _UMSS_CALLBACK_H_

#include "cs_types.h"

#include "uctls_callback.h"

/// @file umss_callback.h
///
/// @addtogroup umss
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


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// g_umssCallback
// -----------------------------------------------------------------------------
/// Mass storage service callback
// =============================================================================
EXPORT PUBLIC CONST UCTLS_SERVICE_CALLBACK_T    g_umssCallback;

/// @} <- End of the umss group

#endif // _UMSS_CALLBACK_H_
