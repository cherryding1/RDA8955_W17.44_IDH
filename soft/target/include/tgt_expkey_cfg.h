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



#ifndef _TGT_EXPKEY_CFG_H_
#define _TGT_EXPKEY_CFG_H_

#include "cs_types.h"
#include "hal_gpio.h"


// =============================================================================
// TGT_CFG_EXPKEY_T
// -----------------------------------------------------------------------------
/// This structure describes the expand key configuration for a given target.
/// The first field identify if expand key is used.
/// The second and third field is the mapped pin when expand key flag is set.
// =============================================================================
typedef struct
{
    /// \c TRUE if expand key is used
    BOOL            expKeyUsed;
    /// The mapped GPO when expKeyUsed is true
    HAL_APO_ID_T    expKeyOut6;
    /// The mapped GPO when expKeyUsed is true
    HAL_APO_ID_T    expKeyOut7;
} TGT_CFG_EXPKEY_T;


// =============================================================================
// tgt_GetExpKeyConfig
// -----------------------------------------------------------------------------
/// This function is used by HAL to get access to expand key configuration.
// =============================================================================
PUBLIC CONST TGT_CFG_EXPKEY_T* tgt_GetExpKeyConfig(VOID);


#endif // _TGT_CAMS_CFG_H_

