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


#ifndef _MEMD_MAP_H_
#define _MEMD_MAP_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// MEMD_BANK_ADDR_SIZE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT32 MEMD_BANK_ADDR_SIZE_T[2];


// ============================================================================
// MEMD_BANK_LAYOUT_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef struct
{
    /// Start address of the bank
    UINT32                         bankStartAddr;                //0x00000000
    MEMD_BANK_ADDR_SIZE_T          sectorLayoutTable[3];         //0x00000004
} MEMD_BANK_LAYOUT_T; //Size : 0x1C



// ============================================================================
// MEMD_FLASH_LAYOUT_T
// -----------------------------------------------------------------------------
/// This structure describes a flash through its different banks.
// =============================================================================
typedef struct
{
    /// Number of banks in the flash
    CONST UINT8                    numberOfBank;                 //0x00000000
    /// Table of pointers to the structures describing the different banks
    CONST MEMD_BANK_LAYOUT_T*      bankLayout;                   //0x00000004
    /// Size of the flash in bytes
    CONST UINT32                   fullSize;                     //0x00000008
    /// User Data Field Base Address
    CONST UINT32                   userDataBase;                 //0x0000000C
    /// User Data Field Size
    CONST UINT32                   userDataSize;                 //0x00000010
    /// User Data Sector Logical Block Size
    CONST UINT32                   userDataSectorLogicalBlockSize; //0x00000014
} MEMD_FLASH_LAYOUT_T; //Size : 0x18



// ============================================================================
// MEMD_MAP_ACCESS_T
// -----------------------------------------------------------------------------
/// Type used to define the accessible structures of the module.
// =============================================================================
typedef struct
{
    UINT32                         ramPhySize;                   //0x00000000
    UINT32                         calibBase;                    //0x00000004
    UINT32                         calibSize;                    //0x00000008
    UINT32                         calibRfBase;                  //0x0000000C
    UINT32                         factBase;                     //0x00000010
    UINT32                         factSize;                     //0x00000014
    MEMD_FLASH_LAYOUT_T*           flashLayout;                  //0x00000018
    UINT32                         flashSize;                    //0x0000001C
    UINT32                         userBase;                     //0x00000020
    UINT32                         userSize;                     //0x00000024
} MEMD_MAP_ACCESS_T; //Size : 0x28






// =============================================================================
// memd_RegisterYourself
// -----------------------------------------------------------------------------
/// This function registers the module itself to HAL so that the version and
/// the map accessor are filled. Then, the CSW get version function and the
/// CoolWatcher get version command will work.
// =============================================================================
PUBLIC VOID memd_RegisterYourself(VOID);



#endif

