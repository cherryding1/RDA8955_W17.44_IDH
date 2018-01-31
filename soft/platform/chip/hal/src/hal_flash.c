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
#include "mem_bridge.h"
#include "hal_ebc.h"
#include "halp_ebc.h"
#include "halp_sys.h"

UINT32 g_halCs0WriteSetting  = 0;

#ifdef CHIP_HAS_EBC_CS2_BUG
PROTECTED UINT32 g_halCs0TimeReg  = 0;
PROTECTED UINT32 g_halCs0ModeReg  = 0;
#endif

// ============================================================================
// hal_EbcFlashWriteEnable
// ----------------------------------------------------------------------------
/// Enables or disables writing for the flash memory accessible through the
/// Chip Select 0.
/// This function is normally reserved for the implementation of flash driver
///
/// @param enable If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC VOID hal_EbcFlashWriteEnable(BOOL enable)
{

#ifndef CHIP_HAS_EBC_CS2_BUG
    UINT32 reg;
#endif

    if (enable)
    {
        g_halCs0WriteSetting = MEM_BRIDGE_WRITEALLOW;
    }
    else
    {
        g_halCs0WriteSetting = 0;
    }

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#else
#ifdef CHIP_HAS_EBC_CS2_BUG
    hwp_memBridge->CS_Time_Write = g_halCs0TimeReg;
    hwp_memBridge->CS_Config[0].CS_Mode = (g_halCs0ModeReg & ~MEM_BRIDGE_WRITEALLOW)
                                          | g_halCs0WriteSetting;
#else
    reg = hwp_memBridge->CS_Config[0].CS_Time;
    hwp_memBridge->CS_Time_Write = reg;

    reg = hwp_memBridge->CS_Config[0].CS_Mode & ~MEM_BRIDGE_WRITEALLOW;
    hwp_memBridge->CS_Config[0].CS_Mode = reg | g_halCs0WriteSetting;

#endif
#endif
}

// ============================================================================
// hal_EbcFlashWriteEnabled
// ----------------------------------------------------------------------------
/// Test if the flash write is enable
///
/// @return If \c TRUE, writing on the CS0 is allowed \n
///               If \c FALSE, the CS0 is read-only
// ============================================================================
PUBLIC BOOL hal_EbcFlashWriteEnabled(VOID)
{
    // If g_halCs0WriteSetting is 0, the bit allowing
    // to write in CS0 is not set.
    return (g_halCs0WriteSetting != 0);
}








