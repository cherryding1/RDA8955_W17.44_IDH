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


#ifndef _BOOTP_DEBUG_H_
#define _BOOTP_DEBUG_H_

#include "cs_types.h"

//#include "debug_port.h"

#include "boot_profile_codes.h"

#include "hal_debug.h"


// =============================================================================
// MACROS
// =============================================================================

#define CPEXITFLAG 0x8000

// =============================================================================
//  BOOT_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_PULSE(pulseName)


// =============================================================================
//  BOOT_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_FUNCTION_ENTER(pulseName)


// =============================================================================
//  BOOT_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define BOOT_PROFILE_FUNCTION_EXIT(pulseName)



// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// FUNCTIONS
// =============================================================================


#endif // _BOOTP_DEBUG_H_
