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





#ifndef _MEMD_M_H_
#define _MEMD_M_H_

#include "cs_types.h"
// =============================================================================
//  Types
// =============================================================================

// =============================================================================
//  MEMD_ERR_T
// -----------------------------------------------------------------------------
/// Error codes
/// The Flash API uses the following error codes.
/// Those errors reflect errors from the flash chip, for consistency the current
/// operation should be restarted.
// =============================================================================
typedef enum
{
    /// No error occured
    MEMD_ERR_NO = 0,

    /// An error occurred during the erase operation
    MEMD_ERR_ERASE = -10,

    /// An error occurred during the write operation
    MEMD_ERR_WRITE,

    /// This error occurs when a command requiring
    /// sector aligned addresses as parameters is called with unaligned addresses.
    /// (Those are
    /// #memd_FlashErrors memd_FlashErase, #memd_FlashLock and #memd_FlashUnlock)
    MEMD_ERR_ALIGN,

    /// An erase or write operation was attempted on a protected sector
    MEMD_ERR_PROTECT,
    /// erase suspend.
    MEMD_ERR_SUSPEND,

} MEMD_ERR_T;


// =============================================================================
// MEMD_BANK_LAYOUT_T
// -----------------------------------------------------------------------------
/// Describes the layout of a flash bank.
// =============================================================================
typedef struct
{
    /// Start address of the bank
    UINT32 bankStartAddr;

    /// Composition of the bank, which can be made of up to 3 different sector
    /// types. One line is for one sector type. The first column is the number
    /// of sectors of this type and the second column is the size of this type
    /// of sector.
    UINT32 sectorLayoutTable[3][2];

} MEMD_BANK_LAYOUT_T;


// =============================================================================
// MEMD_FLASH_LAYOUT_T
// -----------------------------------------------------------------------------
/// This structure describes a flash through its different banks.
// =============================================================================
typedef struct
{
    /// Number of banks in the flash
    CONST UINT8 numberOfBank;

    /// Table of pointers to the structures describing the different banks
    CONST MEMD_BANK_LAYOUT_T* bankLayout;

    /// Size of the flash in bytes.
    CONST UINT32 fullSize;

    /// User Data Field Base Address
    CONST UINT32 userDataBase;

    /// User Data Field Size
    CONST UINT32 userDataSize;

    /// User Data Sector Logical Block Size
    CONST UINT32 userDataSectorLogicalBlockSize;
} MEMD_FLASH_LAYOUT_T;


// =============================================================================
// memd_FlashGetUserFieldInfo
// -----------------------------------------------------------------------------
/// That function get the user data info from memd.def.
///
/// @param userFieldBase The USER_DATA_BASE defined in memd.def.
/// @param userFieldSize The USER_DATA_SIZE defined in memd.def.
/// @param sectorSize The sector size of user data field, it is the large sector size.
///        Some small boot sectors can be regarded as one large sector.
/// @return VOID.
// =============================================================================
PUBLIC VOID memd_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize);

#endif // _MEMD_M_H_
