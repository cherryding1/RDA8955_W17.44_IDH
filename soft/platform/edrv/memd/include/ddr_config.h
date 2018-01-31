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
#ifdef CHIP_DIE_8955

#ifndef _HAL_DDR_CONFIG_H_
#define _HAL_DDR_CONFIG_H_

#include "cs_types.h"
#include "tgt_ddr_cfg.h"

/// During changing DDR PSRAM timing, PSRAM itself can't be accessed. Also,
/// DDR PSRAM timing depends on vcore. So, for vcore change and DDR PSRAM
/// frequency change, the following functions shall be called.


// =============================================================================
// hal_DdrChangeClock
// -----------------------------------------------------------------------------
/// Change DDR PSRAM frequency and change DDR PSRAM timing.
///
/// @param freq         DDR PSRAM freqnency
/// @param usePll       Whether to use PLL divided fast clock
/// @param timing       DDR timing to be updated
// =============================================================================
PUBLIC VOID Ddr_ChangeClock(UINT32 freq, BOOL usePll, HAL_DDR_TIMING_T* timing);

// =============================================================================
// hal_DdrChangeVcore
// -----------------------------------------------------------------------------
/// Change vcore by write PMU registers and change DDR PSRAM timing.
///
/// @param pmuRegCount  PMU register count to be written (<= 8).
/// @param pmuRegIdx    Array of PMU registers index
/// @param pmuRegData   Array of PMU registers data
/// @param timing       DDR timing to be updated
// =============================================================================
PUBLIC VOID Ddr_ChangeVcore(INT32 pmuRegCount,
                            UINT16 *pmuRegIdx,
                            UINT16 *pmuRegData,
                            HAL_DDR_TIMING_T* timing);

#endif // _HAL_DDR_CONFIG_H_
#endif

