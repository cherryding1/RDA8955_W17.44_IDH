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
#include "debug_host.h"
#include "boot.h"
#include "cfg_regs.h"
#include "sys_ctrl.h"
#include "tcu.h"
#include "chip_id.h"


// =============================================================================
// MACROS
// =============================================================================


/// Linker section where the Boot Sector main function must be mapped:
/// Beginning of the first sector of the flash.
#define BOOT_SECTOR_SECTION_START  __attribute__((section (".boot_sector_start")))


// =============================================================================
// FUNCTIONS
// =============================================================================


/// This is defined in "bootp_loader.h" but, this header is not accessible
/// from the ASIC Simu module.
EXPORT PROTECTED VOID boot_LoaderEnter(UINT32 param);


PROTECTED VOID BOOT_SECTOR_SECTION_START asicsimu_BootSector(UINT32 param)
{
    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_UNLOCK;

    // we boot with the boot mode "bypass power up sequence"
    // so we need to switch to the RF clock now !

    // enable RF power & clock
    hwp_configRegs->GPIO_Mode   &= ~CFG_REGS_MODE_PIN_LPSCO_1;
    hwp_tcu->LPS_PU_Ctrl    |= TCU_LPS_PU_CO0_ON | TCU_LPS_PU_CO1_ON | TCU_LPS_PU_CLK_RF_ON | TCU_LPS_PU_SPI_ON | TCU_LPS_PU_CO1_POL;

    // switch to RF clock
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF | SYS_CTRL_SYS_SEL_FAST_SLOW;

    hwp_sysCtrl->REG_DBG    = SYS_CTRL_PROTECT_LOCK;

    boot_LoaderEnter(param);
}

