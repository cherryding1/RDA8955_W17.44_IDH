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


#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)

#ifndef _TGT_DDR_CFG_H_
#define _TGT_DDR_CFG_H_
#include "cs_types.h"


typedef struct
{
    UINT32 ddr_clk_ctrl;
    UINT32 ddr_dqs_ctrl;
    UINT32 mr0;
    UINT32 mr4;
} HAL_DDR_TIMING_T;


typedef struct
{
    UINT8 vcore;
    UINT8 mem_clk;
    HAL_DDR_TIMING_T ddr;
} DDR_TIMING_T;

// =============================================================================
// tgt_GetDDRConfig
// -----------------------------------------------------------------------------
/// This function is used by DDR model to get access to its configuration structure.
// =============================================================================

PUBLIC CONST DDR_TIMING_T* tgt_GetDDRConfig(UINT16* configNum);

#endif // _TGT_MCD_CFG_H_
#endif

