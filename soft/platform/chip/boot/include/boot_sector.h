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


#ifndef _BOOT_SECTOR_H_
#define _BOOT_SECTOR_H_

#include "cs_types.h"
#include "chip_id.h"
#include "sys_ctrl.h"
#include "boot_map.h"
#include "reg_alias.h"


// =============================================================================
// MACROS
// =============================================================================


//=============================================================================
// BOOT_FLASH_PROGRAMMED_PATTERN
//-----------------------------------------------------------------------------
/// Pattern squatting the first word of a properly programmed flash (Or any
/// CS0 plugged device).
//=============================================================================
#define BOOT_FLASH_PROGRAMMED_PATTERN 0xD9EF0045


//=============================================================================
// BOOT_SECTOR_STANDARD_MAIN
//-----------------------------------------------------------------------------
/// Parameter passed to the first function in flash (usually the boot loader).
//=============================================================================
#define BOOT_SECTOR_STANDARD_MAIN 0


/// Value written in the Boot Sector reload structure to say that the ram
/// image present in this structure is valid and can be reloaded.
/// Used for the field validTag.
#define BOOT_SECTOR_RELOAD_VALID_TAG 0xB0075EC6


/// The bit to check in the Boot Mode register to decide if the Boot Sector
/// structure is valid or not and its address has been put in the variable
/// Boot Sector structure pointer.
/// This define is used by HAL Boot Sector driver and by the Boot Sector code.
/// Caution: the g_bootBootMode is shifted by 16 bits compared to the register.
#define BOOT_SECTOR_STRUCT_VALID_BOOT_MODE       SYS_CTRL_SW_BOOT_MODE(1 << 0)

#define BOOT_SECTOR_SET_STRUCT_VALIDITY()        (hwp_sysCtrl->Reset_Cause |= \
                                                 BOOT_SECTOR_STRUCT_VALID_BOOT_MODE)
#define BOOT_SECTOR_GET_STRUCT_VALIDITY()        (g_bootBootMode & \
                                                 BOOT_SECTOR_STRUCT_VALID_BOOT_MODE >> 16)
#define BOOT_SECTOR_CLR_STRUCT_VALIDITY()        (hwp_sysCtrl->Reset_Cause &= \
                                                 ~BOOT_SECTOR_STRUCT_VALID_BOOT_MODE; \
                                                 g_bootBootMode &= \
                                                 ~BOOT_SECTOR_STRUCT_VALID_BOOT_MODE >> 16)


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// BOOT_SECTOR_RELOAD_ID_T
// -----------------------------------------------------------------------------
/// List of Boot Sector reload sector Id.
// =============================================================================
typedef enum
{
    BOOT_SECTOR_RELOAD_HOST_CFG_ID = 0,
    BOOT_SECTOR_RELOAD_INTSRAM_ID,
    BOOT_SECTOR_RELOAD_SRAM_ID,
    BOOT_SECTOR_RELOAD_MAX_NUM
} BOOT_SECTOR_RELOAD_ID_T;


// =============================================================================
// BOOT_SECTOR_RELOAD_CTX_T
// -----------------------------------------------------------------------------
/// boot sector reload context
/// this structure contain the strore address, map address and size of sector in the ram image .
// =============================================================================
typedef struct
{
    UINT32 storeAddress;
    UINT32 mapAddress;
    UINT32 size;
} BOOT_SECTOR_RELOAD_CTX_T;


// =============================================================================
// BOOT_SECTOR_RELOAD_STRUCT_T
// -----------------------------------------------------------------------------
/// Structure type used by the Boot Sector to determine when to reload the ram image
/// This structure contains information written by HAL during the normal
/// execution of the code and used by the Boot Sector to reload the ram image
/// when the phone boots.
// =============================================================================
typedef struct
{
    UINT32 validTag;
    UINT32 checkSum;
    BOOT_SECTOR_RELOAD_CTX_T ctx[BOOT_SECTOR_RELOAD_MAX_NUM];
} BOOT_SECTOR_RELOAD_STRUCT_T;


// =============================================================================
// FUNCTIONS
// =============================================================================


/// Should only be used by inline function boot_BootSectorGetStructPointer().
extern BOOT_SECTOR_STRUCT_T* g_bootSectorStructPtr;


/// Should only be used by inline function boot_BootSectorGetReloadPointer().
extern BOOT_SECTOR_RELOAD_STRUCT_T* g_bootSectorReloadStructPtr;


// =============================================================================
// FUNCTIONS
// =============================================================================


// =============================================================================
// boot_BootSectorGetStructPointer
// -----------------------------------------------------------------------------
/// Return the address of the Boot Sector structure pointer.
/// The Boot Sector structure pointer is a global variable located in Internal
/// SRAM, at a fixed location. It contains the address of the Boot Sector
/// structure as filled by HAL Boot Sector driver at normal execution time
/// and used by the Boot Sector code at boot time.
///
/// @return Boot Sector structure pointer address.
// =============================================================================
PUBLIC INLINE BOOT_SECTOR_STRUCT_T** boot_BootSectorGetStructPointer(VOID)
{
    return &g_bootSectorStructPtr;
}


// =============================================================================
// boot_BootSectorGetReloadPointer
// -----------------------------------------------------------------------------
/// Return the address of the Boot Sector reload structure pointer.
/// The Boot Sector reload structure pointer is a global variable located in Internal
/// SRAM, at a fixed location. It contains the address of the Boot Sector reload
/// structure as filled by HAL Boot Sector reload driver at
/// normal execution time and used by the Boot Sector code to reload the ram image
/// at boot time.
///
/// @return Boot Sector reload structure pointer address.
// =============================================================================
PUBLIC INLINE BOOT_SECTOR_RELOAD_STRUCT_T** boot_BootSectorGetReloadStructPointer(VOID)
{
    return &g_bootSectorReloadStructPtr;
}


// =============================================================================
// boot_Sector
// -----------------------------------------------------------------------------
/// Entry of the Boot Sector in the first flash sector.
///
/// This code is executed right after the Boot ROM and right before the
/// Boot Loader, in normal mode execution. It does the following:
///
/// * Writes the EBC RAM chip select configuration if a valid configuration
/// has been found in the Boot Sector structure.
///
/// * If the command in the Boot Sector structure tells us to enter
/// boot monitor, we do so after sending an event.
///
/// * Else, we enter the Boot Loader, providing it with the #param.
///
/// @param param Generic parameter only used to invoke boot_LoaderEnter(param).
// =============================================================================
PROTECTED VOID boot_Sector(UINT32 param);


#endif // _BOOT_SECTOR_H_
