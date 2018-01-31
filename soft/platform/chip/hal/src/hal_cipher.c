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


#include "cs_types.h"

#include "global_macros.h"
#include "cipher.h"

#include "hal_cipher.h"
#include "hal_error.h"

// =============================================================================
// hal_CipherKcInit
// -----------------------------------------------------------------------------
/// Initialize Kc
/// @param kcLow   Low part of the Kc Key
/// @param kcHigh  High part of the Kc Key
/// @return #HAL_ERR_RESOURCE_BUSY if the ciphering is already running or
/// #HAL_ERR_NO if the initialization is properly done.
// =============================================================================

PUBLIC UINT8 hal_CipherKcInit(UINT32 kcLow,    UINT32 kcHigh)
{
    UINT32 error;

    //  Check if the cipher A5 is already processing
    error = hwp_cipher->status & CIPHER_RUNNING;
    if (error)
    {
        return HAL_ERR_RESOURCE_BUSY;
    }
    else
    {
        //  Write low bits of Kc register
        hwp_cipher->Kc_low = kcLow;
        //  Write high bits of Kc register
        hwp_cipher->Kc_high = kcHigh;
        return HAL_ERR_NO;
    }
}
