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
#include "dbg.h"
#include "stdarg.h"
#include "hal_debug.h"

// =============================================================================
// dbg_AssertUtil
// -----------------------------------------------------------------------------
/// Utilitary function to define the assert.
/// This function is used in the public macro that implements the minimal side-
/// effect assert.
/// @param format String format a-la printf, displayed in the trace.
// =============================================================================
PUBLIC VOID dbg_AssertUtil(CONST CHAR* format, ...)
{
    va_list args;
    va_start(args, format);
    // Just a HAL wrapper.
    hal_DbgAssert(format, args);
    va_end(args);
}
