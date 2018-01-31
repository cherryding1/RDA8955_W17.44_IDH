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



#ifndef _TGT_BTD_CFG_H_
#define _TGT_BTD_CFG_H_

#include "cs_types.h"


typedef struct BTD_CONFIG_STRUCT_T TGT_BTD_CONFIG_T;

// =============================================================================
// tgt_GetBtdConfig
// -----------------------------------------------------------------------------
/// This function is used by BTD to get access to its configuration structure.
/// This is the only way BTD can get this information.
// =============================================================================
PUBLIC CONST TGT_BTD_CONFIG_T* tgt_GetBtdConfig(VOID);


#endif // _TGT_BTD_CFG_H_
