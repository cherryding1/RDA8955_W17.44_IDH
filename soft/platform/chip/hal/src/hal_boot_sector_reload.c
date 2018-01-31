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

#include "boot_sector.h"
#include "halp_debug.h"



// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

/// Boot Sector reload structure.
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector to reload the ram image
/// when the phone boots.
PRIVATE BOOT_SECTOR_RELOAD_STRUCT_T
HAL_BOOT_SECTOR_RELOAD_STRUCT_SECTION g_halBootSectorReloadStruct;



// =============================================================================
// FUNCTIONS
// =============================================================================



// =============================================================================
// hal_BootSectorFillReloadCtx
// -----------------------------------------------------------------------------
/// Fill the boot sector reload context.
/// ctx: pointer to the boot sector reload context.
/// id: boot sector reload id
// =============================================================================
PUBLIC BOOL hal_BootSectorFillReloadCtx(BOOT_SECTOR_RELOAD_CTX_T *ctx, BOOT_SECTOR_RELOAD_ID_T id)
{
    if(id >= BOOT_SECTOR_RELOAD_MAX_NUM)
    {
        HAL_ASSERT(FALSE,"boot sector reload: unkown id %d",id);
        return FALSE;
    }

    g_halBootSectorReloadStruct.ctx[id].storeAddress = ctx->storeAddress;
    g_halBootSectorReloadStruct.ctx[id].mapAddress = ctx->mapAddress;
    g_halBootSectorReloadStruct.ctx[id].size = ctx->size;

    return TRUE;
}

// =============================================================================
// hal_BootSectorReloadEnable
// -----------------------------------------------------------------------------
/// Validate the Boot Sector reload structure. This is a way to tell the Boot Sector
/// that the structure contains correct ram image and that it must be loaded
/// and executed when restart.
///
/// This is done by putting the address of the Boot Sector reload structure in
/// the Boot Sector reload structure pointer and enabling a flag in it.
// =============================================================================
PUBLIC VOID hal_BootSectorReloadEnable(VOID)
{
    g_halBootSectorReloadStruct.validTag = BOOT_SECTOR_RELOAD_VALID_TAG;
    g_halBootSectorReloadStruct.checkSum = ~BOOT_SECTOR_RELOAD_VALID_TAG;
    *boot_BootSectorGetReloadStructPointer() = &g_halBootSectorReloadStruct;
}


