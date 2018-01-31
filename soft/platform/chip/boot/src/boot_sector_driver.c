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


#include "chip_id.h"
#include "global_macros.h"

#include "cs_types.h"

#include "hal_mem_map.h"
#include "boot_sector_driver.h"

#include "boot_sector.h"


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// HAL_BOOT_SECTOR_STRUCT_SECTION
// -----------------------------------------------------------------------------
/// Macro used to put the Boot Sector structure at a fixed location in internal
/// SRAM. This will avoid it to be overwritten by a RAMRUN, for instance.
// =============================================================================
#define BOOT_SECTOR_STRUCT_SECTION __attribute__((section(".boot_sector_struct")))



// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/// Boot Sector structure.
///
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector when the phone boots.
///
/// It is mapped in the Internal SRAM because we don't want the Boot
/// Sector code to access the External RAM. And it is mapped at a fixed
/// location (at the place of the last patch) because we don't want the RAMRUN
/// to overwrite it.
PRIVATE BOOT_SECTOR_STRUCT_T
BOOT_SECTOR_STRUCT_SECTION g_bootBootSectorStruct;


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// boot_BootSectorValidateStruct
// -----------------------------------------------------------------------------
/// Validate the Boot Sector structure. This is a way to tell the Boot Sector
/// that the structure contains correct data and that it must be read and
/// that the commands that it contains must be executed.
///
/// This is done by putting the address of the Boot Sector structure in
/// the Boot Sector structure pointer and enabling a bit in the Boot Mode
/// register.
// =============================================================================
PRIVATE VOID boot_BootSectorValidateStruct(VOID)
{
    *boot_BootSectorGetStructPointer() = &g_bootBootSectorStruct;
    BOOT_SECTOR_SET_STRUCT_VALIDITY();
}


// =============================================================================
// boot_BootSectorSetCommand
// -----------------------------------------------------------------------------
/// Set the command which must be executed by the Boot Sector.
/// Then validate the Boot Sector structure.
///
/// @param valid Command used during the next boot.
// =============================================================================
PUBLIC VOID boot_BootSectorSetCommand(BOOT_SECTOR_CMD_T command)
{
    g_bootBootSectorStruct.command = command;
    boot_BootSectorValidateStruct();
}


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
/// @param timings EBC RAM chip select timing configuration as defined by
/// the EBC CS_Time_Write register.
/// @param mode EBC RAM chip select mode configuration as defined by the
/// EBC CS_Mode register.
// =============================================================================
PUBLIC VOID boot_BootSectorSetEbcConfig(UINT32 timings, UINT32 mode)
{
    g_bootBootSectorStruct.ebcConfigRamTimings = timings;
    g_bootBootSectorStruct.ebcConfigRamMode    = mode;
    g_bootBootSectorStruct.ebcConfigValidTag   = BOOT_SECTOR_EBC_VALID_TAG;
    boot_BootSectorValidateStruct();
}


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
PUBLIC VOID boot_BootSectorSetEbcFreq(HAL_SYS_MEM_FREQ_T ebcFreq)
{
    g_bootBootSectorStruct.ebcFreq = ebcFreq;
    g_bootBootSectorStruct.ebcExtConfigValidTag = BOOT_SECTOR_EBC_EXT_VALID_TAG;
}


// =============================================================================
// boot_BootSectorGetEbcFreq
// -----------------------------------------------------------------------------
/// Get the EBC frequency in the EBC extension configuration.
///
/// @return the EBC frequency.
// =============================================================================
PUBLIC HAL_SYS_MEM_FREQ_T boot_BootSectorGetEbcFreq(VOID)
{
    return g_bootBootSectorStruct.ebcFreq;
}


// =============================================================================
// boot_BootSectorSetEbcRamId
// -----------------------------------------------------------------------------
/// Save the external RAM ID to select a correct timing.
///
/// @param ramId the external RAM ID.
// =============================================================================
PUBLIC VOID boot_BootSectorSetEbcRamId(UINT8 ramId)
{
    g_bootBootSectorStruct.ramId = ramId;
}


// =============================================================================
// boot_BootSectorGetEbcRamId
// -----------------------------------------------------------------------------
/// Get the external RAM ID.
///
/// @return the external RAM ID.
// =============================================================================
PUBLIC UINT8 boot_BootSectorGetEbcRamId(VOID)
{
    return g_bootBootSectorStruct.ramId;
}


