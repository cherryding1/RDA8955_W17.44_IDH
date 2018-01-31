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

#ifndef _GPSD_M_H
#define _GPSD_M_H

#include "cs_types.h"



#define GPSD_MALLOC COS_MALLOC
#define GPSD_FREE COS_FREE

// ============================================================================
// TYPES
// ========================================================================

// =============================================================================
// GPSD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type GPSD_CONFIG_T for configuration
// ========================================================================
typedef struct GPSD_CONFIG_STRUCT_T GPSD_CONFIG_T;


EXTERN_C_START

EXTERN_C_END

///  @} <- End of the GPSD group

#endif  // _GPSD_M_H_


