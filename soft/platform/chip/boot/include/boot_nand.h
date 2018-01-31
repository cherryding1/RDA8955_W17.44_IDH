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



#ifndef _BOOT_NAND_H_
#define _BOOT_NAND_H_

/// @defgroup BOOT NAND Driver
///
/// @{
///

#include "cs_types.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// boot_NandOpen
// -----------------------------------------------------------------------------
/// Initialize NAND flash controller
/// @param None
/// @return None
// =============================================================================

PUBLIC VOID boot_NandOpen(VOID);


// =============================================================================
// boot_NandGetPageSize
// -----------------------------------------------------------------------------
/// Get the page size of the NAND flash
/// @param None
/// @return The page size
// =============================================================================

PUBLIC UINT32 boot_NandGetPageSize(VOID);


// =============================================================================
// boot_NandReadByPageNum
// -----------------------------------------------------------------------------
/// Read one page of data from NAND flash by page number
/// @param page    The page number in NAND flash
/// @param pData   Pointing to a buffer to hold the data (should be word-aligned)
/// @param len     The number of bytes to be read (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_NandReadByPageNum(UINT32 page, UINT32 *pData, UINT32 len);


// =============================================================================
// boot_NandReadPages
// -----------------------------------------------------------------------------
/// Read multiple page data from NAND flash
/// @param addr    The start address in NAND flash (should be page-aligned)
/// @param pData   Pointing to a buffer to hold the data (should be word-aligned)
/// @param len     The number of bytes to be read (should be mulitple of 4)
/// @return None
// =============================================================================

PUBLIC VOID boot_NandReadPages(UINT32 addr, UINT32 *pData, UINT32 len);


/// @} <-- End of the nand group

#endif // _BOOT_NAND_H_

