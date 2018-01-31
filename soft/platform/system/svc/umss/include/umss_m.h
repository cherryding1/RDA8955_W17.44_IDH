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


#ifndef _UMSS_M_H_
#define _UMSS_M_H_

#include "cs_types.h"

#include "hal_usb.h"

#include "umss_config.h"

/// @file umss_m.h
/// @mainpage USB Mass Storage service
///
/// This service provides Mass Storage service on usb
///
/// This document is composed of:
/// - @ref umss
///
/// @addtogroup umss USB Mass Storage Service (umss)
/// @{
///

// =============================================================================
// MACROS
// =============================================================================

#ifndef UMSS_STORAGE_MAX
#define UMSS_STORAGE_MAX 4
#endif /* UMSS_STORAGE_MAX */

// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// FUNCTIONS
// =============================================================================

typedef INT32 (*UFLASH_WRTBLK_FUNC_P)( UINT32 iPartId, UINT32 iVBNum, UINT8*pBuff );
typedef INT32 (*UFLASH_RDBLK_FUNC_P)( UINT32 iPartId, UINT32 iVBNum, UINT8* pBuff );

void init_uFlash_Func (UFLASH_WRTBLK_FUNC_P  pf_wrtblk_func,
                       UFLASH_RDBLK_FUNC_P  pf_rdblk_func);
void init_uFlash_Param(UINT32 devNo, UINT32 nrBlock, UINT32 blkSize);
///  @} <- End of the umss group

#endif // _UMSS_M_H_
