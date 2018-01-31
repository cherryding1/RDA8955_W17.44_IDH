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

#ifndef _MCIP_DEBUG_H_
#define _MCIP_DEBUG_H_


// =============================================================================
//  TRACE Level mapping
// -----------------------------------------------------------------------------
/// Important trace that can be enabled all the time (low rate)
#define MCI_WARN_TRC       (_MCI | TLEVEL(1))
/// AUDIO specific trace
#define MCI_AUDIO_TRC        (_MCI | TLEVEL(2))
/// VIDEO specific trace
#define MCI_VIDEO_TRC       (_MCI | TLEVEL(3))
/// VIDEO RECORDER specific trace
#define MCI_VIDREC_TRC       (_MCI | TLEVEL(4))
/// VIDEO RECORDER specific trace
#define MCI_CAMERA_TRC       (_MCI | TLEVEL(5))
/// MALLOC specific trace
#define MCI_MALLOC_TRC       (_MCI | TLEVEL(6))
/// AnalogTV specific trace
#define MCI_ANALOGTV_TRC       (_MCI | TLEVEL(7))

// add more here
/// reserved for debug (can be very verbose, but should probably not stay in code)
#define MCI_DBG_TRC        (_MCI | TLEVEL(16))

// =============================================================================
//  TRACE
// -----------------------------------------------------------------------------
/// Trace macro to use to send a trace. The parameter \c format is a string
/// containing parameters in the "print fashion", but limited for trace to 6
/// parameters. The parameter \c tstmap defines which parameter is a string.
/// (Leave it to 0, if you don't use %s in fmt);
// =============================================================================
// Conditional Printf Usage
#ifndef MCI_NO_TRACE
#define MCI_TRACE(level, tstmap, format, ...)  hal_DbgTrace(_MCI | level,tstmap,format, ##__VA_ARGS__)
#else
#define MCI_TRACE(level, tstmap, format, ...)
#endif //MCI_NO_PRINTF



#ifdef MCI_NO_ASSERT
#define MCI_ASSERT(boolCondition, format, ...)
#else
// =============================================================================
//  ASSERT
// -----------------------------------------------------------------------------
/// Assert: If the boolean condition (first parameter) is true,
/// raise an assert and print the decorated string passed in the other
/// parameter.
// =============================================================================
#define MCI_ASSERT(boolCondition, format, ...)                   \
    if ((boolCondition)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif





//#define diag_printf(format,...) MCI_TRACE(TSTDOUT | TLEVEL(1), 0, format, ##__VA_ARGS__)
#define diag_printf(format,...) MCI_TRACE(MCI_VIDEO_TRC, 0, format, ##__VA_ARGS__)

#define ass( boolCondition )     MCI_ASSERT(boolCondition, "pls view GDB")
#define assinfo( boolCondition, format,... )      MCI_ASSERT(boolCondition, format,##__VA_ARGS__)

#define convertaddr(x) (uint32 *)((uint32)(x) | 0xa0000000)



#endif

//#define malloc(x) (malloc(x) ? : ass(1));

