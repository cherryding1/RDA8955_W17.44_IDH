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



#ifndef _TGT_MEMD_CFG_H_
#define _TGT_MEMD_CFG_H_

#include "cs_types.h"
#include "memd_config.h"



// =============================================================================
// tgt_GetMemdFlashConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Flash configuration structure.
// =============================================================================
PUBLIC CONST MEMD_FLASH_CONFIG_T* tgt_GetMemdFlashConfig(VOID);



// =============================================================================
// tgt_GetMemdRamConfig
// -----------------------------------------------------------------------------
/// This function is used to get access to the Ram configuration structure.
// =============================================================================
PUBLIC CONST MEMD_RAM_CONFIG_T* tgt_GetMemdRamConfig(VOID);

#endif // _TGT_MEMD_CFG_H_

