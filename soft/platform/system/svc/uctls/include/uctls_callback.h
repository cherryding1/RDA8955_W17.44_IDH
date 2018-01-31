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


#ifndef _UCTLS_CALLBACK_H_
#define _UCTLS_CALLBACK_H_

#include "cs_types.h"


//#include "uctls_m.h"
#include "uvideos_m.h"


#include "umss_m.h"

#include "uwifi_m.h" //
#include "uat_m.h"
#include "uat_config.h"
#include "urndis_m.h"


/// @file uctls_callback.h
///
/// @addtogroup uctls
/// @{
///

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// UCTLS_SERVICE_CFG_T
// -----------------------------------------------------------------------------
/// Service configuration data
// =============================================================================
typedef union
{
    CONST UMSS_CFG_T mss;
    CONST UVIDEOS_CFG_T videos;
    CONST UAT_CFG_T at;
    CONST URNDIS_CFG_T rndis;
    CONST UWIFI_CFG_T wifi; //USB_WIFI_SUPPORT
} UCTLS_SERVICE_CFG_T;

// =============================================================================
// UCTLS_SERVICE_CALLBACK_OPEN_T
// -----------------------------------------------------------------------------
/// Open service callback
/// @param config service configure structure
/// @return USB list interface descriptor
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T**
(*UCTLS_SERVICE_CALLBACK_OPEN_T)(CONST UCTLS_SERVICE_CFG_T* config);

// =============================================================================
// UCTLS_SERVICE_CALLBACK_CLOSE_T
// -----------------------------------------------------------------------------
/// Close service callback
// =============================================================================
typedef VOID
(*UCTLS_SERVICE_CALLBACK_CLOSE_T)(VOID);

// =============================================================================
// UCTLS_SERVICE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback service structure
// =============================================================================
typedef struct
{
    // Open
    CONST UCTLS_SERVICE_CALLBACK_OPEN_T  open;
    // Close
    CONST UCTLS_SERVICE_CALLBACK_CLOSE_T close;
} UCTLS_SERVICE_CALLBACK_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================

/// @} <- End of the uctls group

#endif // _UCTLS_CALLBACK_H_

