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



#include "cs_types.h"

#include "string.h"
#include <stdarg.h>
#include "stdio.h"

#include "hal_host.h"
#include "hal_sys.h"
#ifdef CHIP_HAS_AP
#include "hal_ap_comm.h"
#endif

#include "halp_debug.h"
#include "boot.h"

#include "sxs_io.h"


#define HAL_ASSERT_ERROR_FIFO_SIZE       32
#define HAL_ASSERT_CODE              0xa55E
#define HAL_ASSERT_DELAY             100000
#define HAL_ASSERT_LOOP_QTY          20


// =============================================================================
// hal_DbgAssert
// -----------------------------------------------------------------------------
/// Function to call when 'building' an assert, if the condition is false.
/// @param format String format a-la printf, displayed in the trace.
// =============================================================================
PUBLIC VOID hal_DbgAssert(CONST CHAR* format, ...)
{
    va_list args;
    CHAR msg[256];

    va_start(args, format);
    vsprintf(msg, format, args);

    hal_HstSendEvent(0xA55E47);
    // We never go out of this function, so we can safely assume
    // it is possible to read it from coolwatcher.
    hal_HstSendEvent((UINT32)msg);
#ifdef CHIP_HAS_AP
    hal_ApCommSaveExcInfo((UINT32)msg, sizeof(msg));
#endif
    HAL_PROFILE_PULSE(XCPU_ERROR);
    hal_DbgFatalTrigger(HAL_DBG_EXL_FATAL);

    xcpu_error_code = HAL_ASSERT_CODE;
    xcpu_error_status = 0;

    //  Print string to display
    // TSMAP is the bitfield of parameters which are string
    // sxs_Raise is a no-exit function
    UINT32 raiseId = _PAL|TSMAP(0x1)|TABORT;
    sxs_Raise(raiseId,
              "\n##############################################\n"
              "# FATAL assert \n"
              "#    --> %s\n"
              "##############################################",
              (format==NULL? "/no string/":(CHAR *)msg));

//  ...and at last end loop
    while (1);

    va_end(args);
}


