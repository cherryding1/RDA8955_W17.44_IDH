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


#ifndef _BOOT_SECTOR_CHIP_H_
#define _BOOT_SECTOR_CHIP_H_


#include "global_macros.h"
#ifdef CT_ASM
#include "mem_bridge_asm.h"
#else
#include "mem_bridge.h"
#endif


//=============================================================================
// BOOT_SECTOR_ENTRY_POINT
//-----------------------------------------------------------------------------
/// First address where to jump in flash, to execute code
//=============================================================================
#define BOOT_SECTOR_ENTRY_POINT (KSEG0(REG_CS0_BASE)+0x10)


//=============================================================================
// BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS
//-----------------------------------------------------------------------------
/// Address of the programmed flash pattern.
//=============================================================================
#define BOOT_FLASH_PROGRAMMED_PATTERN_ADDRESS ((UINT32*)KSEG0(REG_CS0_BASE))


#endif // _BOOT_SECTOR_CHIP_H_


