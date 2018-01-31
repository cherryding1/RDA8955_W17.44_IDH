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



#ifndef _TGT_HAL_CFG_H
#define _TGT_HAL_CFG_H

#include "cs_types.h"
#include "hal_config.h"


// =============================================================================
// tgt_GetHalConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to its configuration structure.
/// This is the only way HAL can get this information.
// =============================================================================
PUBLIC CONST HAL_CFG_CONFIG_T* tgt_GetHalConfig(VOID);


#endif // _TGT_HAL_CFG_H

