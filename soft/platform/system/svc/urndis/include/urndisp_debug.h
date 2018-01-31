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

#ifndef _URNDISP_DEBUG_H_
#define _URNDISP_DEBUG_H_

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
#define URNDIS_WARN_TRC   SVC_WARN_TRC

/// Informational trace interesting for URNDIS only
#define URNDIS_INFO_TRC   SVC_UVIDEOS_TRC//share with SVC_UVIDEOS_TRC level
/// Informational trace interesting for URNDIS data only
#define URNDIS_DATA_TRC   SVC_UVIDEOS_TRC//share with SVC_UVIDEOS_TRC level

/// reserved for debug (can be very verbose, but should probably not stay in code)
#define URNDIS_DBG_TRC    SVC_DBG_TRC

#ifndef URNDIS_NO_PRINTF
#define URNDIS_TRACE(level, tsmap, format, ...) \
    do \
    { \
            dbg_TraceOutputText(0, (format), ##__VA_ARGS__); \
    } while (0)
#else
#define URNDIS_TRACE(level, tsmap, fmt, ...)
#endif // URNDIS_NO_PRINTF

// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _URNDISP_DEBUG_H_

