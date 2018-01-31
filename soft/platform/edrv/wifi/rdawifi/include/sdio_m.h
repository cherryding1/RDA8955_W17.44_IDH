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



#ifndef _SDIO_M_H_
#define _SDIO_M_H_

#include "cs_types.h"

/// @file mcd_m.h

/// This document describes the characteristics of the  SD/MMC
/// Driver and how to use it via its API.

/// @defgroup mcd EDRV Memory Card Driver (MCD)
///
/// This document describes the characteristics of the Granite MMC Driver
/// and how to use it via its API.
///
/// The MMC is a flash memory memory card standard. This driver provides a
/// basic API to configure, read and write data on a MMC through the Granite
/// Hardware Abstraction Layer SPI.
///
/// @par Important Note
///
/// @par MMC Operation
/// The details on how to use the MMC are written in the <B> Application Note
/// 0025 "How to write and read data to MMC with SPI" </B>.
///
/// @par
/// The MMC can be connected either on #HAL_SPI_CS0 or #HAL_SPI_CS1.
///
/// @par
/// The clock of the SPI to which the MMC is connected can be calculated by:
/// Clock_Spi = Clock_System/(2*(Clock_divider+1)), where \c clock_divider is
/// a parameter used to open the MMC.
///
/// @par CSD Register
/// The CSD is the Card Specific Data register. It contains configuration
/// information required to access the card data. For more information about
/// CSD register, refers to your MMC specification.
///
/// @par
/// This information is returned by the function used to open the MMC.
///
/// @par Transfer mode
/// In SPI mode, the MMC supports only block data read and write operation.
/// The max block length supported by the card is given by the CSD register
/// field \c WRITE_BL_LEN.
///
/// @par
/// The #mcd_Write is implemented by using MMC standard commands \c CMD16
/// \c SET_BLOCKLEN and \c CMD24 \c WRITE_BLOCK, so the constraints of these
/// commands should also be met. Specifically, the data transferred must not
/// cross a physical boundary. For more details, please refer to your MMC
/// specification.
///
/// @par
/// The #mcd_Read is implemented by using MMC standard commands \c CMD16
/// \c SET_BLOCKLEN and \c CMD17 \c READ_SINGLE_BLOCK, so the constraints of
/// these commands should also be met. For more details, please refer to your
/// MMC specification.
/// @{
///


// =============================================================================
// Macro
// =============================================================================



// =============================================================================
//  Types
// =============================================================================

// =============================================================================
// MCD_CSD_T
// -----------------------------------------------------------------------------
/// This structure contains the fields of the MMC chip's register.
/// For more details, please refer to your MMC specification.
// =============================================================================
typedef struct
{
    // Ver 2. // Ver 1.0 (if different)
    UINT8   csdStructure;           // 127:126
    UINT8   specVers;                   // 125:122
    UINT8   taac;                   // 119:112
    UINT8   nsac;                   // 111:104
    UINT8   tranSpeed;              // 103:96
    UINT16  ccc;                    //  95:84
    UINT8   readBlLen;              //  83:80
    BOOL    readBlPartial;          //  79:79
    BOOL    writeBlkMisalign;       //  78:78
    BOOL    readBlkMisalign;        //  77:77
    BOOL    dsrImp;                 //  76:76
    UINT32  cSize;                  //  69:48 // 73:62
    UINT8   vddRCurrMin;            //           61:59
    UINT8   vddRCurrMax;            //           58:56
    UINT8   vddWCurrMin;             //           55:53
    UINT8   vddWCurrMax;            //           52:50
    UINT8   cSizeMult;              //           49:47
    // FIXME
    UINT8   eraseBlkEnable;
    UINT8   eraseGrpSize;           //  ??? 46:42
    // FIXME
    UINT8   sectorSize;
    UINT8   eraseGrpMult;           //  ??? 41:37

    UINT8   wpGrpSize;              //  38:32
    BOOL    wpGrpEnable;            //  31:31
    UINT8   defaultEcc;             //  30:29
    UINT8   r2wFactor;              //  28:26
    UINT8   writeBlLen;             //  25:22
    BOOL    writeBlPartial;         //  21:21
    BOOL    contentProtApp;         //  16:16
    BOOL    fileFormatGrp;          //  15:15
    BOOL    copy;                   //  14:14
    BOOL    permWriteProtect;       //  13:13
    BOOL    tmpWriteProtect;        //  12:12
    UINT8   fileFormat;             //  11:10
    UINT8   ecc;                    //   9:8
    UINT8   crc;                    //   7:1
    /// This field is not from the CSD register.
    /// This is the actual block number.
    UINT32  blockNumber;
} MCD_CSD_T;


// =============================================================================
// MCD_ERR_T
// -----------------------------------------------------------------------------
/// Type used to describe the error status of the MMC driver.
// =============================================================================
typedef enum
{
    MCD_ERR_NO = 0,
    MCD_ERR_CARD_TIMEOUT = 1,
    MCD_ERR_DMA_BUSY = 3,
    MCD_ERR_CSD = 4,
    MCD_ERR_SPI_BUSY = 5,
    MCD_ERR_BLOCK_LEN = 6,
    MCD_ERR_CARD_NO_RESPONSE,
    MCD_ERR_CARD_RESPONSE_BAD_CRC,
    MCD_ERR_CMD,
    MCD_ERR_UNUSABLE_CARD,
    MCD_ERR_NO_CARD,
    MCD_ERR_NO_HOTPLUG,

    /// A general error value
    MCD_ERR,
} MCD_ERR_T;

// =============================================================================
// MCD_STATUS_T
// -----------------------------------------------------------------------------
/// Status of card
// =============================================================================
typedef enum
{
    // Card present and mcd is open
    MCD_STATUS_OPEN,
    // Card present and mcd is not open
    MCD_STATUS_NOTOPEN_PRESENT,
    // Card not present
    MCD_STATUS_NOTPRESENT,
    // Card removed, still open (please close !)
    MCD_STATUS_OPEN_NOTPRESENT
} MCD_STATUS_T ;

// =============================================================================
// MCD_CARD_SIZE_T
// -----------------------------------------------------------------------------
/// Card size
// =============================================================================
typedef struct
{
    UINT32 nbBlock;
    UINT32 blockLen;
} MCD_CARD_SIZE_T ;


// =============================================================================
// MCD_CARD_VER
// -----------------------------------------------------------------------------
/// Card version
// =============================================================================

typedef enum
{
    MCD_CARD_V1,
    MCD_CARD_V2
} MCD_CARD_VER;


// =============================================================================
// MCD_CARD_ID
// -----------------------------------------------------------------------------
/// Card version
// =============================================================================

typedef enum
{
    MCD_CARD_ID_0,
    MCD_CARD_ID_1,
    MCD_CARD_ID_NO,
} MCD_CARD_ID;

EXTERN_C_START

// =============================================================================
// MCD_CARD_DETECT_HANDLER_T
// -----------------------------------------------------------------------------
/// Type for card insertion/removal detection
/// @param cardPlugged \c TRUE when a card is inserted,
///                    \c FALSE when a card is removed
// =============================================================================
typedef VOID (*MCD_CARD_DETECT_HANDLER_T)(BOOL cardPlugged);

// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// mcd_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC MCD_ERR_T mcd_SetCardDetectHandler(MCD_CARD_DETECT_HANDLER_T handler);

// =============================================================================
// mcd_Open
// -----------------------------------------------------------------------------
/// Open the MMC/SD chip
/// This function does the init process of the MMC chip, including reseting
/// the chip, sending a command of init to MMC, and reading the CSD
/// configurations.
///
/// @param mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
/// @param mcdCsd Pointer to the structure where the MMC CSD register info
/// are going to be written.
///
/// @param mcdVer is t card version.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer);

// =============================================================================
// mcd_CardStatus
// -----------------------------------------------------------------------------
/// Return the card status
///
/// @return Card status see #MCD_STATUS_T
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
PUBLIC MCD_STATUS_T mcd_CardStatus(MCD_CARD_ID mcdId);
#else
PUBLIC MCD_STATUS_T mcd_CardStatus(VOID);
#endif

// =============================================================================
// mcd_GetCardSize
// -----------------------------------------------------------------------------
/// Get card size
///
/// @param size Structure use to store size of memory card
// =============================================================================
PUBLIC MCD_ERR_T mcd_GetCardSize(MCD_CARD_SIZE_T* size);

// =============================================================================
// mcd_Write
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
PUBLIC MCD_ERR_T mcd_Write(UINT32 startAddr, CONST UINT8* blockWr, UINT32 size);


// =============================================================================
// mcd_Read
// -----------------------------------------------------------------------------
/// Read using pattern mode.
/// @param startAddr: of the MMC memory block where the data
/// will be read
/// @param blockRd Pointer to the buffer where the data will be stored. Must be aligned
/// on a 32 bits boundary.
/// @param size Number of bytes to read. Must be an interger multiple of the
/// sector size of the card.
// =============================================================================
PUBLIC MCD_ERR_T mcd_Read(UINT32 startAddr, UINT8* blockRd, UINT32 size);


// =============================================================================
// mcd_Close
// -----------------------------------------------------------------------------
/// Close MCD.
///
/// To be called at the end of the operations
/// @param mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
/// @return MCD_ERR_NO if a response with a good crc was received,
///         MCD_ERR_CARD_NO_RESPONSE if no reponse was received within the
/// driver configured timeout.
///          MCD_ERR_CARD_RESPONSE_BAD_CRC if the received response presented
///  a bad CRC.
///         MCD_ERR_CARD_TIMEOUT if the card timedout during procedure.
// =============================================================================
#ifdef DUAL_TFLASH_SUPPORT
//PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId);
#else
//PUBLIC MCD_ERR_T mcd_Close(VOID);
#endif

//PUBLIC VOID mcd_Enter_LowPower(VOID);

#ifdef DUAL_TFLASH_SUPPORT
// =============================================================================
// mcd_SwitchOperationTo
// -----------------------------------------------------------------------------
/// Switch MCD id.
///
/// To be called when appliction switch to another t-falsh.
/// @param  mcdId is mcd device identification under dual t-flash mode.value is 0 or 1.
///
// =============================================================================
PUBLIC MCD_ERR_T mcd_SwitchOperationTo(MCD_CARD_ID mcdId);
#endif

//PUBLIC UINT8 mcd_IsOpened(MCD_CARD_ID mcdId);
EXTERN_C_END
/// @} <- End of the mmc group

#endif // _SDIO_M_H_

