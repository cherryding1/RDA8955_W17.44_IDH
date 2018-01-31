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


#ifndef _BOOT_SECTOR_DRIVER_H_
#define _BOOT_SECTOR_DRIVER_H_

#include "cs_types.h"

#include "chip_id.h"

#include "boot_map.h"
#include "boot_sys.h"

// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// boot_BootSectorSetCommand
// -----------------------------------------------------------------------------
/// Set the command which must be executed by the Boot Sector.
/// Then validate the Boot Sector structure.
///
/// @param valid Command used during the next boot.
// =============================================================================
PUBLIC VOID boot_BootSectorSetCommand(BOOT_SECTOR_CMD_T command);


// =============================================================================
// boot_BootSectorSetEbcConfig
// -----------------------------------------------------------------------------
/// Set the configuration of the EBC for the RAM chip select. This
/// configuration will be written by the Boot Sector code to be sure the
/// RAM configured properly after a reset (in this case, the RAM chip
/// might not get the system reset). Without this, a RAM configured in burst
/// mode could be in an unknown state.
///
/// Then validate the EBC configuration and the Boot Sector structure.
///
/// This function is *forced* inline because it will be used by HAL and by
/// the Boot Sector and the linkage of HAL and Boot Sector can be independent.
///
/// @param timings EBC RAM chip select timing configuration as defined by
/// the EBC CS_Time_Write register.
/// @param mode EBC RAM chip select mode configuration as defined by the
/// EBC CS_Mode register.
// =============================================================================
PUBLIC VOID boot_BootSectorSetEbcConfig(UINT32 timings, UINT32 mode);


// =============================================================================
// boot_BootSectorSetEbcFreq
// -----------------------------------------------------------------------------
/// Save EBC frequency for the RAM chip select. This
/// configuration will be written by the Boot Sector code to be sure the
/// RAM configured properly after a reset (in this case, the RAM chip
/// might not get the system reset). Without this, a RAM configured in burst
/// mode could be in an unknown state.
///
/// Then validate the EBC extension configuration.
///
/// @param ebcFreq the EBC frequency.
// =============================================================================
PUBLIC VOID boot_BootSectorSetEbcFreq(HAL_SYS_MEM_FREQ_T ebcFreq);


// =============================================================================
// boot_BootSectorGetEbcFreq
// -----------------------------------------------------------------------------
/// Get the EBC frequency in the EBC extension configuration.
///
/// @return the EBC frequency.
// =============================================================================
PUBLIC HAL_SYS_MEM_FREQ_T boot_BootSectorGetEbcFreq(VOID);


// =============================================================================
// boot_BootSectorSetEbcRamId
// -----------------------------------------------------------------------------
/// Save the external RAM ID to select a correct timing.
///
/// @param ramId the external RAM ID.
// =============================================================================
PUBLIC VOID boot_BootSectorSetEbcRamId(UINT8 ramId);


// =============================================================================
// boot_BootSectorGetEbcRamId
// -----------------------------------------------------------------------------
/// Get the external RAM ID.
///
/// @return the external RAM ID.
// =============================================================================
PUBLIC UINT8 boot_BootSectorGetEbcRamId(VOID);


#endif // _BOOT_SECTOR_DRIVER_H_
