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


#ifndef  _HALP_ANA_GPADC_H_
#define  _HALP_ANA_GPADC_H_

#include "cs_types.h"
#include "hal_ana_gpadc.h"




// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =======================================================
// hal_AnaGpadcSleep
// -------------------------------------------------------
/// called in #hal_LpsSleep to try to shut down the GPADC
/// and release the resource. (only if atp >= 250MS)
// =======================================================
PROTECTED VOID hal_AnaGpadcSleep(VOID);

// =======================================================
// hal_AnaGpadcWakeUp
// -------------------------------------------------------
/// called in #hal_LpsSleep to resume the GPADC operation
/// can take back the resource (if GPADC is open)
// =======================================================
PROTECTED VOID hal_AnaGpadcWakeUp(VOID);



#endif // _HALP_ANA_GPADC_H_



