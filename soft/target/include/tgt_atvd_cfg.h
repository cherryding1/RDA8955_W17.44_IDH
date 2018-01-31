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



#ifndef _TGT_ATVD_CFG_H_
#define _TGT_ATVD_CFG_H_

#include "cs_types.h"


typedef struct ATVD_CONFIG_STRUCT_T TGT_ATVD_CONFIG_T;

// =============================================================================
// tgt_GetATVDConfig
// -----------------------------------------------------------------------------
/// This function is used by ATVD to get access to its configuration structure.
/// This is the only way ATVD can get this information.
// =============================================================================
PUBLIC CONST TGT_ATVD_CONFIG_T* tgt_GetAtvdConfig(VOID);


#endif // _TGT_ATVD_CFG_H_
