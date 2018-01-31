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

#ifndef _UWIFIP_DEBUG_H_
#define _UWIFIP_DEBUG_H_

#include "cs_types.h"
#include "svcp_debug.h"

// =============================================================================
// MACROS
// =============================================================================

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
// =============================================================================
#define UWIFI_WARN_TRC   SVC_WARN_TRC

/// Informational trace interesting for AVRS only
#define UWIFI_INFO_TRC   SVC_UWIFI_TRC

/// reserved for debug (can be very verbose, but should probably not stay in code)
#define UWIFI_DBG_TRC    SVC_DBG_TRC

#ifndef UWIFI_NO_PRINTF
#define UWIFI_TRACE(level,tsmap,format,...)(level, tsmap, format, ...)   SVC_TRACE(level,tsmap,format, ##__VA_ARGS__)
#else
#define UWIFI_TRACE(level, tsmap, fmt, ...)
#endif // UVIDEOS_NO_PRINTF

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _UVIDEOSP_DEBUG_H_

