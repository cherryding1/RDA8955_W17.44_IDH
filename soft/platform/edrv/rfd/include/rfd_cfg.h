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

#ifndef RFD_CFG_H
#define RFD_CFG_H


#include "edrvp_debug.h"
// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define RFD_WARN_TRC EDRV_WARN_TRC
/// Informational trace interesting for RFD only
#define RFD_INFO_TRC EDRV_RFD_TRC
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define RFD_DBG_TRC EDRV_DBG_TRC

// -----------------------------
// TRACE CONFIGURATION
// -----------------------------
#ifndef RFD_NO_TRACE
#include "cs_types.h"
#include "sxs_io.h"
#if (USE_GCC_4 == 1)
//=====================================================
// GCC 4 always puts string constants in .rodata section and will not encounter
// the GCC 3 issue.
//=====================================================
#define RFD_TRACE(level, tstmap, format, ...)  \
      hal_DbgTrace(_EDRV | level,tstmap,format, ##__VA_ARGS__)
#define rfd_fprintf(id, fmt, ...)    \
    SXS_TRACE(id, fmt, ##__VA_ARGS__)
#else // (USE_GCC_4 != 1)
//=====================================================
// Bug in GCC 3 for -mips16 option:
// String constants are always put in .text section. If they are local variables,
// they will be referenced via the offsets based on .text section.
// This will lead to an error when the codes are located in a section other than .text.
//--------------------------------------------------------------------
// Workaround:
// When the codes are located in a section other than .text, define string constants
// as global variables (static variables can work as well).
//--------------------------------------------------------------------
// Limitation:
// No TDB macro is allowed, as trace function is expecting string other than trace ID.
//=====================================================
#define RFD_TRACE(level, tstmap, format, ...)  \
    { const static char* const rfdTrcStr = format; \
       hal_DbgTrace(_EDRV | level,tstmap,rfdTrcStr, ##__VA_ARGS__); }
#define rfd_fprintf(id, fmt, ...)    \
    { const static char* const rfdTrcStr = fmt; \
       SXS_TRACE(id, rfdTrcStr, ##__VA_ARGS__); }
#endif  // (USE_GCC_4 != 1)
#else // RFD_NO_TRACE
#define RFD_TRACE(level, tsmap, fmt, ...)
#define rfd_fprintf(id, fmt, ...)
#endif //RFD_NO_TRACE

// -----------------------------
// ASSERT CONFIGURATION
// -----------------------------
#ifndef RFD_NO_ASSERT

#include "hal_debug.h"
// define something with this one
#define RFD_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }


#else

#define RFD_ASSERT(BOOL, format, ...)

#endif // RFD_NO_ASSERT

// -----------------------------
// COOLPROFILE CONFIGURATION
// -----------------------------
#ifndef RFD_NO_COOLPROFILE

#include "hal_debug.h"
#include "rfd_profile_codes.h"
#define RFD_CP_ENTRY        hal_DbgPxtsProfileFunctionEnter
#define RFD_CP_EXIT         hal_DbgPxtsProfileFunctionExit
#define RFD_CP_PULSE        hal_DbgPxtsProfilePulse
#define RFD_CP_WINENTRY     hal_DbgPxtsProfileWindowEnter
#define RFD_CP_WINEXIT      hal_DbgPxtsProfileWindowExit

#else

#define RFD_CP_ENTRY
#define RFD_CP_EXIT
#define RFD_CP_PULSE
#define RFD_CP_WINENTRY
#define RFD_CP_WINEXIT

#endif

// -----------------------------
// CALIBRATION CONFIGURATION
// -----------------------------
#ifdef CALIB
#define RFD_CALIB
#endif // CALIBRATION

#endif // RFD_CFG_H

