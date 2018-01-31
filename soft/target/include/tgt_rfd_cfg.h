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



#ifndef _TGT_RFD_CFG_H
#define _TGT_RFD_CFG_H

#include "cs_types.h"
#include "rfd_config.h"


// =============================================================================
// tgt_GetXcvConfig
// -----------------------------------------------------------------------------
/// This function is used by XCV to get access to its configuration structure.
/// This is the only way XCV can get this information.
// =============================================================================
PUBLIC CONST XCV_CONFIG_T* tgt_GetXcvConfig(VOID);


// =============================================================================
// tgt_GetPaConfig
// -----------------------------------------------------------------------------
/// This function is used by PA to get access to its configuration structure.
/// This is the only way PA can get this information.
// =============================================================================
PUBLIC CONST PA_CONFIG_T*  tgt_GetPaConfig(VOID);


// =============================================================================
// tgt_GetSwConfig
// -----------------------------------------------------------------------------
/// This function is used by SW to get access to its configuration structure.
/// This is the only way SW can get this information.
// =============================================================================
PUBLIC CONST SW_CONFIG_T*  tgt_GetSwConfig(VOID);

#endif // _TGT_RFD_CFG_H

