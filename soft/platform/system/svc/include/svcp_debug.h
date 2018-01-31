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


#include "hal_debug.h"
#include "sxs_io.h"

#include "cos.h"
#ifndef _SVCP_DEBUG_H_
#define _SVCP_DEBUG_H_

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define SVC_WARN_TRC        (_SVC1 | TLEVEL(1))

/// AVCTLS specific trace
#define SVC_AVCTLS_TRC      (_SVC1 | TLEVEL(2))

/// AVPS specific trace
#define SVC_AVPS_TRC        (_SVC1 | TLEVEL(3))

/// AVRS specific trace
#define SVC_AVRS_TRC        (_SVC1 | TLEVEL(4))

/// CAMS specific trace
#define SVC_CAMS_TRC        (_SVC1 | TLEVEL(5))

/// FMG specific trace
#define SVC_FMG_TRC         (_SVC1 | TLEVEL(6))

/// FSS specific trace
#define SVC_FSS_TRC         (_SVC1 | TLEVEL(7))

/// IMGS specific trace
#define SVC_IMGS_TRC        (_SVC1 | TLEVEL(8))

/// MPS specific trace
#define SVC_MPS_TRC         (_SVC1 | TLEVEL(9))

/// MRS specific trace
#define SVC_MRS_TRC         (_SVC1 | TLEVEL(10))

/// UCTLS specific trace
#define SVC_UCTLS_TRC       (_SVC1 | TLEVEL(11))

/// UMSS specific trace
#define SVC_UMSS_TRC        (_SVC1 | TLEVEL(12))

/// UTRACES specific trace
#define SVC_UTRACES_TRC     (_SVC1 | TLEVEL(13))

/// VOIS specific trace
#define SVC_VOIS_TRC        (_SVC1 | TLEVEL(14))

/// UVIDEOS specific trace
#define SVC_UVIDEOS_TRC     (_SVC1 | TLEVEL(15))

/// SYSCMDS specific trace
#define SVC_SYSCMDS_TRC     (_SVC1 | TLEVEL(16))

/// VPP specific trace
#define SVC_VPP_TRC         (_SVC2 | TLEVEL(1))

// add more here


/// APS specific trace
#define SVC_APS_TRC         (_SVC2 | TLEVEL(14))

/// ARS specific trace
#define SVC_ARS_TRC         (_SVC2 | TLEVEL(15))

/// reserved for debug (can be very verbose, but should probably not stay in code)
#define SVC_DBG_TRC         (_SVC2 | TLEVEL(16))


// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
#ifndef SVC_NO_TRACE
#define SVC_TRACE(level, tstmap, format, ...)  hal_DbgTrace(level,tstmap,format, ##__VA_ARGS__)
#else
#define SVC_TRACE(level, tstmap, format, ...)
#endif //SVC_NO_TRACE

#define USB_TRACE_ID (TID(_SVC1) | TLEVEL(11))
#define USBLOGV(...) COS_TRACE_V(USB_TRACE_ID, __VA_ARGS__)
#define USBLOGD(...) COS_TRACE_D(USB_TRACE_ID, __VA_ARGS__)
#define USBLOGI(...) COS_TRACE_I(USB_TRACE_ID, __VA_ARGS__)
#define USBLOGW(...) COS_TRACE_W(USB_TRACE_ID, __VA_ARGS__)
#define USBLOGE(...) COS_TRACE_E(USB_TRACE_ID, __VA_ARGS__)
#define USBLOGSV(tsmap, ...) COS_TRACE_V(USB_TRACE_ID | (tsmap), __VA_ARGS__)
#define USBLOGSD(tsmap, ...) COS_TRACE_D(USB_TRACE_ID | (tsmap), __VA_ARGS__)
#define USBLOGSI(tsmap, ...) COS_TRACE_I(USB_TRACE_ID | (tsmap), __VA_ARGS__)
#define USBLOGSW(tsmap, ...) COS_TRACE_W(USB_TRACE_ID | (tsmap), __VA_ARGS__)
#define USBLOGSE(tsmap, ...) COS_TRACE_E(USB_TRACE_ID | (tsmap), __VA_ARGS__)
#define USBDUMP(data, size) COS_DUMP(USB_TRACE_ID, data, size);
#define USBDUMP16(data, size) COS_DUMP16(USB_TRACE_ID, data, size);
#define USBDUMP32(data, size) COS_DUMP32(USB_TRACE_ID, data, size);


#endif // _SVCP_DEBUG_H_

