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

#ifndef _EDRVP_DEBUG_H_
#define _EDRVP_DEBUG_H_

// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define EDRV_WARN_TRC       (_EDRV | TLEVEL(1))
/// PMD specific trace
#define EDRV_PMD_TRC        (_EDRV | TLEVEL(2))
/// MEMD specific trace
#define EDRV_MEMD_TRC       (_EDRV | TLEVEL(3))
/// RFD specific trace
#define EDRV_RFD_TRC        (_EDRV | TLEVEL(4))
/// AUD specific trace
#define EDRV_AUD_TRC        (_EDRV | TLEVEL(5))
/// LCDD specific trace
#define EDRV_LCDD_TRC       (_EDRV | TLEVEL(6))
/// MCD specific trace
#define EDRV_MCD_TRC        (_EDRV | TLEVEL(7))
/// CAMD specific trace
#define EDRV_CAMD_TRC       (_EDRV | TLEVEL(8))
/// FMD specific trace
#define EDRV_FMD_TRC        (_EDRV | TLEVEL(9))
/// BTD specific trace
#define EDRV_BTD_TRC        (_EDRV | TLEVEL(10))
/// TSD specific trace
#define EDRV_TSD_TRC        (_EDRV | TLEVEL(11))
/// GSS specific trace
#define EDRV_GSS_TRC        (_EDRV | TLEVEL(12))
/// GPIOI2C specific trace
#define EDRV_GI2C_TRC       (_EDRV | TLEVEL(13))
/// SIMD specific trace
#define EDRV_SIMD_TRC       (_EDRV | TLEVEL(14))
// add more here
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define EDRV_DBG_TRC        (_EDRV | TLEVEL(16))

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
#ifndef EDRV_NO_TRACE
#define EDRV_TRACE(level, tstmap, format, ...)  hal_DbgTrace(_EDRV | level,tstmap,format, ##__VA_ARGS__)
#else
#define EDRV_TRACE(level, tstmap, format, ...)
#endif //EDRV_NO_PRINTF



#endif // _EDRVP_DEBUG_H_

