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



#ifndef _MCD_SDMMC_H_
#define _MCD_SDMMC_H_

#include "cs_types.h"


// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// mcd_sdmmc_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_SetCardDetectHandler(MCD_CARD_DETECT_HANDLER_T handler);


// =============================================================================
// mcd_sdmmc_Open
// -----------------------------------------------------------------------------
/// Open the MMC/SD chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
///
/// @param mcdVer is t card version.
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_Open(MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer,MCD_SDMMC_SPI_IF* mcdConfig);


// =============================================================================
// mcd_sdmmc_CardDetectUpdateStatus
// -----------------------------------------------------------------------------
/// Return the card status
///
/// @return Card status see #MCD_STATUS_T
// =============================================================================
PUBLIC VOID mcd_sdmmc_CardDetectHandler(VOID);

// =============================================================================
// mcd_sdmmc_GetCardSize
// -----------------------------------------------------------------------------
/// Get card size
///
/// @param size Structure use to store size of memory card
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_GetCardSize(MCD_CARD_SIZE_T* size);

// =============================================================================
// mcd_sdmmc_Write
// -----------------------------------------------------------------------------
/// Write a block of data to MMC.
///
/// This function is used to write blocks of data on the MMC.
/// @param startAddr Start Adress  of the MMC memory block where the
/// data will be written
/// @param blockWr Pointer to the block of data to write. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to write. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_Write(UINT32 startAddr, CONST UINT8* blockWr, UINT32 size);


// =============================================================================
// mcd_sdmmc_Read
// -----------------------------------------------------------------------------
/// Read using pattern mode.
/// @param startAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to read. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_Read(UINT32 startAddr, UINT8* blockRd, UINT32 size);


// =============================================================================
// mcd_sdmmc_Close
// -----------------------------------------------------------------------------
/// Close MCD.
///
/// To be called at the end of the operations
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================
PUBLIC MCD_ERR_T mcd_sdmmc_Close(VOID);

// =============================================================================
// mcd_sdmmc_CardStatus
// -----------------------------------------------------------------------------
/// Return the card status
///
/// @return Card status see #MCD_STATUS_T
// =============================================================================
PUBLIC MCD_STATUS_T mcd_sdmmc_CardStatus(VOID);


#ifdef DUAL_TFLASH_SUPPORT
PUBLIC MCD_ERR_T mcd_sdmmc_SwitchOperationTo(UINT8 card_number);
#endif



PUBLIC MCD_ERR_T mcd_sdmmc_GetCardSize(MCD_CARD_SIZE_T* size);
PUBLIC MCD_ERR_T mcd_sdmmc_GetWRBlockSize(UINT32* size);

/// @} <- End of the sdmmc group

#endif // _MCD_M_H_







