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

#include "mcd_m.h"
#include "mcdp_debug.h"

// =============================================================================
// mcd_Open
// -----------------------------------------------------------------------------
/// @brief Open the SD-MMC chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer)
{
    MCD_ASSERT(FALSE, "NULL mcd_Open doesn't exist !");
    return MCD_ERR_NO;
}


// =============================================================================
// mcd_Close
// -----------------------------------------------------------------------------
/// Close MCD.
///
/// To be called at the end of the operations
// =============================================================================
PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId)
{
    MCD_ASSERT(FALSE, "NULL mcd_Close doesn't exist !");
    return MCD_ERR_NO;
}

// =============================================================================
// mcd_Write
// -----------------------------------------------------------------------------
/// @brief Write a block of data to MMC.
///
/// This function is used to write blocks of data on the MMC.
/// @param blockStartAddr Start Adress  of the MMC memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to write. Must be an interger multiple of the
/// sector size of the card
// =============================================================================
PUBLIC MCD_ERR_T mcd_Write(UINT32 blockStartAddr, CONST UINT8* blockWr, UINT32 size)
{
    MCD_ASSERT(FALSE, "NULL mcd_Write doesn't exist !");
    return MCD_ERR_NO;
}



// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// @brief Read using pattern mode.
/// @ param blockStartAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to read. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* blockRd, UINT32 size)
{
    MCD_ASSERT(FALSE, "NULL mcd_Read doesn't exist !");
    return MCD_ERR_NO;
}

