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



#ifndef _TGT_TSD_CFG_H_
#define _TGT_TSD_CFG_H_

#include "cs_types.h"
#include "hal_spi.h"


// =============================================================================
// tgt_GetTsdConfig
// -----------------------------------------------------------------------------
/// This function is used by MCD to get access to its configuration structure.
/// This is the only way TSD can get this information.
// =============================================================================
PUBLIC CONST TSD_CONFIG_T* tgt_GetTsdConfig(VOID);


// =============================================================================
// tgt_GetTsdKeyAdcValue
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to the TSD key ADC values.
// =============================================================================
PUBLIC VOID tgt_GetTsdKeyAdcValue(CONST UINT16 **pArray, UINT8 *pLen);


// =============================================================================
// tgt_GetTsdKey2AdcValue
// -----------------------------------------------------------------------------
/// This function is used by TSD to get access to the TSD key2 ADC values.
// =============================================================================
PUBLIC VOID tgt_GetTsdKey2AdcValue(CONST UINT16 **pArray, UINT8 *pLen);


#endif // _TGT_TSD_CFG_H_

