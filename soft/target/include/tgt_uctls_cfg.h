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



#ifndef _TGT_UCTLS_CFG_H_
#define _TGT_UCTLS_CFG_H_

#include "cs_types.h"

#include "uctls_m.h"

#include "utraces_callback.h"
#include "umss_callback.h"
#include "umss_config.h"
#include "uvideos_callback.h"
#include "uvideos_config.h"
#include "uat_callback.h"
#include "uat_config.h"
#include "urndis_callback.h"
#include "urndis_config.h"
#include "uwifi_callback.h"
#include "uwifi_config.h"

// =============================================================================
// UCTLS_SERVICE_T
// -----------------------------------------------------------------------------
/// Usb Service
// =============================================================================
typedef struct
{
    // Service Callback
    CONST UCTLS_SERVICE_CALLBACK_T* serviceCallback;
    // Service configuration
    CONST UCTLS_SERVICE_CFG_T       serviceCfg;
} UCTLS_SERVICE_T;

// =============================================================================
// UCTLS_SERVICE_LIST_T
// -----------------------------------------------------------------------------
/// List of services
// =============================================================================
typedef struct
{
    CONST UCTLS_SERVICE_T servicesList[UCTLS_ID_MAX];
} UCTLS_SERVICE_LIST_T;

// =============================================================================
// UCTLS_CONFIG_T
// -----------------------------------------------------------------------------
/// Rename type for uctls config
// =============================================================================
typedef UCTLS_SERVICE_LIST_T UCTLS_CONFIG_T;

// =============================================================================
// tgt_GetUctlsConfig
// -----------------------------------------------------------------------------
/// This function is used by UCTL to get access to its configuration structure.
/// This is the only way UCTLS can get this information.
// =============================================================================
PUBLIC CONST UCTLS_CONFIG_T* tgt_GetUctlsConfig(VOID);

#endif // _TGT_UCTLS_CFG_H_
