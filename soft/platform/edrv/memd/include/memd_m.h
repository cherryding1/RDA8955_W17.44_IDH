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
#include "memd_config.h"
#include "memd_map.h"
#include "hal_sys.h"
/// @}


/// @defgroup ramd EDRV MEMD RAM Driver
/// This driver only consists in the memd_RamOpen open function,
/// which configure the RAM to offer its best possible use. Accessing
/// RAM before a call to this function is possible, but will offer
/// poor performance.
/// @{



/// @} // <-- End of the ramd group

/// @defgroup flashd EDRV MEMD Flash Driver
///
/// This document describes the characteristics of the Flash Driver
/// and how to use it its API
///
/// A flash chip can be used for both program and file system data. This driver
/// provide a basic API for accessing generic flash chips to implement a file
/// system.
///
/// @par Flash Operation
/// This driver provides an abstraction of the flash commands. Nevertheless, the
/// sector structure of the flash must still be managed at the upper software
/// level.
///
/// @par
/// A basic flash handling is proposed by the HAL layer. But to fully use
/// the flash at its best performance, a call to the #memd_Open function
/// must be made, to set the specific driver configuration.
///
/// @par
/// Also note that in a flash chip, bit can only go from '1' to '0' during a
/// write operation. To get back to '1', a full sector need to be erased. This
/// must be taken into account by the upper level software. For instance
/// managing a block list of used sector should be coded so that it writes
/// only '0' to change the state of a sector from free to use, and from used to
/// erasable.
///
/// @par
/// Moreover the life time of a sector is counted in number of erases, so
/// trying to distribute the use of sectors before erasing them will improve
/// the global lifetime of the flash. (Anyway, a file system should also be
/// able to mark sectors as erroneous).
///
/// @par
/// The driver basic functions access the flash as a block device. Note that
/// the read and write functions can actually access at byte boundaries.
///
///
/// @{
///

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
typedef enum
{
    DDR_FREQ_26M ,
    DDR_FREQ_39M ,
    DDR_FREQ_52M ,
    DDR_FREQ_78M ,
    DDR_FREQ_89M ,
    DDR_FREQ_104M,
    DDR_FREQ_113M,
    DDR_FREQ_125M,
    DDR_FREQ_139M,
    DDR_FREQ_156M,
    DDR_FREQ_178M,
    DDR_FREQ_208M,
    DDR_FREQ_250M,
    DDR_FREQ_312M,
} DRV_DDR_FREQ_T;


PUBLIC BOOL  memd_AdjustRamClk(HAL_SYS_MEM_FREQ_T ddrFreq,BOOL usePll);
#endif
// =============================================================================
//  Defines
// =============================================================================

// Macro used to put a specific function in flash boot area
#define MEMD_BOOT_FUNC __attribute__((section(".boottext")))

typedef enum
{
    HAL_FLASH_FREQ_13M,
    HAL_FLASH_FREQ_26M,
    HAL_FLASH_FREQ_39M,
    HAL_FLASH_FREQ_52M,
    HAL_FLASH_FREQ_78M,
    HAL_FLASH_FREQ_89M,
    HAL_FLASH_FREQ_104M,
    HAL_FLASH_FREQ_QTY,
} HAL_FLASH_FREQ_T;

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
    /// device is busy.
    MEMD_ERR_BUSY,
    MEMD_ERR_SPI_BUSY,
} MEMD_ERR_T;

typedef enum
{
    FLASH_STATUS_IDLE,
    FLASH_STATUS_BUSY,
    FLASH_STATUS_ERASE,
    FLASH_STATUS_W_IDLE,
    FLASH_STATUS_W_PROG,
    FLASH_STATUS_W_BEGIN,
    FLASH_STATUS_W_WRITTING,
} FLASH_STATUS;
// =============================================================================

#define SECOND        * HAL_TICK1S
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND

#define HAL_FUNC_RAM __attribute__((section (".ram")))


#ifdef _FLASH_PROGRAMMER
extern u8 sxr_NewSemaphore(u8 InitValue);
extern void sxr_TakeSemaphore(u8 Id);
extern void sxr_ReleaseSemaphore(u8 Id);
#endif

extern BOOL HAL_FUNC_RAM hal_GetVocDma(void);
extern BOOL HAL_FUNC_RAM hal_DmaDone(VOID);
extern BOOL hal_LpsRemainingFramesReady(UINT32 frameNum);
extern VOID mon_Event(UINT32 evt);


// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// memd_RamOpen
// -----------------------------------------------------------------------------
/// That function configures the RAM.
/// It must be called to configure the best settings for the RAM, thus enabling
/// the fatest access.
/// @param cfg Ram configuration used to open the RAM.
///
// =============================================================================
PUBLIC VOID memd_RamOpen(CONST MEMD_RAM_CONFIG_T* cfg);


// =============================================================================
// memd_FlashOpen
// -----------------------------------------------------------------------------
/// That function configures the flash.
/// It must be called before using any other function of the MEMD API.
///
/// @param cfg Pointer to the configuration to use to open the flash.
/// This parameter is ignored by the romulator driver.
/// @return Pointer towards the structure describing the flash.
// =============================================================================
PUBLIC MEMD_FLASH_LAYOUT_T* memd_FlashOpen(CONST MEMD_FLASH_CONFIG_T* cfg);

PUBLIC VOID memd_FlashRst(VOID);


// =============================================================================
// memd_FlashRead
// -----------------------------------------------------------------------------
/// This function reads data from the flash. It gets its data from \c flashAddress,
/// copies \c bytesize bytes to \c buffer. \c preadbytesize is filled with the
/// actual number of read bytes (Equal to \c bytesize, or less in case of error)
///
/// @param flashAddress The byte offset within the flash chip (Take care of
/// the  positions of the code in the flash)
/// @param byteSize Number of bytes to read from the flash
/// @param pReadByteSize Filled with the number of bytes actually read
/// @param buffer Buffer to fill with data from flash
/// @return #MEMD_ERR_NO
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashRead(UINT8 *flashAddress,
                                 UINT32  byteSize,
                                 UINT32* pReadByteSize,
                                 UINT8*  buffer);

// =============================================================================
// memd_FlashWrite
// -----------------------------------------------------------------------------
/// This function writes data in the flash. It gets its data from \c buffer, copies
/// \c bytesize bytes to the flash location designed by \c flashAddress. \c
/// pwrittenbytesize is filled with the actual number of written bytes (Equal
/// to size, or less in case of error).
///
/// @param  flashAddress The byte offset within the flash chip. (Take care not
/// to overwrite the code present at the beginning of the flash)
/// @param  byteSize Number of bytes to write in flash
/// @param  pWrittenByteSize Number of bytes actually written in flash
/// @param  buffer Buffer where to get the data to write in the flash
/// @return #MEMD_ERR_NO, #MEMD_ERR_WRITE or #MEMD_ERR_PROTECT
/// whether the operation succeeded or failed.
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashWrite(UINT8 *flashAddress,
                                  UINT32 byteSize,
                                  UINT32 * pWrittenByteSize,
                                  CONST UINT8* buffer);


// =============================================================================
// memd_FlashErase
// -----------------------------------------------------------------------------
/// This function erases contiguous flash sectors.
/// Addresses are byte offset within the flash chip and <B> must be aligned on sectors</B>:
/// - The \c startFlashAddress is the address of the first sector to erase.
/// - The \c endFlashAddress is the address of the first sector NOT to erase.
///   If \c endFlashAddress is \c NULL, only one sector will be erased.
/// .
/// Care must be taken not to erase the code present at the beginning of the flash.
///
/// @param start_flashAddress The address of the first sector to erase
/// @param end_flashAddress The address of the first sector NOT to erase.
///   If \c NULL, only one sector will be erased
/// @return #MEMD_ERR_NO, #MEMD_ERR_ERASE, #MEMD_ERR_ALIGN or #MEMD_ERR_PROTECT
/// whether the operation succeeded or failed
///
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashErase(
    UINT8 *startFlashAddress,
    UINT8 *endFlashAddress);

// =============================================================================
// memd_FlashErase2
// -----------------------------------------------------------------------------
/// This function erases contiguous flash sectors.
/// Addresses are byte offset within the flash chip and <B> must be aligned on sectors</B>:
/// - The \c startFlashAddress is the address of the first sector to erase.
/// - The \c endFlashAddress is the address of the first sector NOT to erase.
/// .
/// Care must be taken not to erase the code present at the beginning of the flash.
/// Comparing to memd_FlashErase, block erase will be chosen if possible (flash address
/// is block aligned and size is larger or equal then block size), and flash
/// layout won't be considered
///
/// @param start_flashAddress The address of the first sector to erase
/// @param end_flashAddress The address of the first sector NOT to erase.
/// @return #MEMD_ERR_NO, #MEMD_ERR_ERASE, #MEMD_ERR_ALIGN or #MEMD_ERR_PROTECT
/// whether the operation succeeded or failed
///
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashEraseNOSectorLimit(
    UINT8 *startFlashAddress,
    UINT8 *endFlashAddress);

// =============================================================================
// memd_FlashGetGlobalAddress
// -----------------------------------------------------------------------------
/// This function returns the global address in the system memory map
/// from the flash relative address.
/// Using the returned address, the user will be able to read directly
/// from the flash, without using #memd_FlashRead
///
/// Note that the flash can be read directly, but it cannot be written without
/// using the memd driver functions. That's why the returned address is declared
/// as \c CONST. The content of the flash can be erased without the help of the
/// CPU (by sending a command to the flash chip), that's why the returned
/// address is declared as \c VOLATILE.
///
/// @param flashAddress Byte address within the flash chip to be converted
/// into a global byte address (take care to the positions of the code in flash)
/// @return The byte global address
// =============================================================================
PUBLIC VOLATILE CONST UINT8* memd_FlashGetGlobalAddress(UINT8 *flashAddress);


PUBLIC UINT32 memd_FlashGetSectorLimits(UINT32 sectorId, UINT32 *start, UINT32 *end);


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


// =============================================================================
// memd_FlashRomulatorSetRamMode
// -----------------------------------------------------------------------------
/// Set the Romulator in RAM mode. This function is only effective when using
/// a romulator device. When using a real Flash Combo, the function returns
/// \c FALSE and does nothing.
///
/// When the romualtor is in RAM mode, it can be accessed as a RAM, that is to
/// say read and written without sending any command, but through direct
/// addressing.
///
/// @param  enable Enable or disable the RAM mode.
/// @return \c TRUE if the operation could be done and the switch done,
///         \c FALSE otherwise.
// =============================================================================
PUBLIC BOOL memd_FlashRomulatorSetRamMode(BOOL enable);


PUBLIC VOID memd_FlashSetCtrRegionSize(UINT32 CtrSize);

PUBLIC UINT32 memd_FlashGetCtrRegionSize(VOID);

PUBLIC VOID memd_FlashSetFSSectorCTXSizeInfo(UINT32 sectorCTXSize);
PUBLIC UINT32 memd_FlashGetFSSectorCTXSizeInfo(VOID);

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
PUBLIC  MEMD_ERR_T memd_FlashErase_Respond_Irq(UINT8 *startFlashAddress, UINT8 *endFlashAddress,UINT32 time);
#ifdef __VDS_QUICK_FLUSH__
PUBLIC MEMD_ERR_T memd_FlashWrite_Respond_Irq(UINT8 *flashAddress,UINT32 byteSize, UINT32 * pWrittenByteSize,UINT8* buffer);
VOID memd_SetFramsIrqIgnore(BOOL Ignore);
BOOL memd_SysIsIdle(void);
#endif
PUBLIC VOID memd_SuspendEnable(VOID);
PUBLIC VOID memd_SuspendDisable(VOID);
PUBLIC BOOL memd_SuspendIsEnable(VOID);
#endif

PUBLIC UINT32 memd_FlashErase2SuspendTime(VOID);

PUBLIC int memd_FlashCodeSectionOtp(UINT8 protect_section, BOOL otp_val);
PUBLIC void memd_Flash_CalibWriteFlashFlag(BOOL writeFlag);

//SPI flash: SPI register bit define
#ifdef __PRJ_WITH_SPIFLSH__
/*
[16 14...13]size: 0000: 8Mbit     0001: 16Mbit     0010: 32Mbit      0011: 64Mbit
                1000:128Mbit     1001:256Mbit
[12]chip num: 0: 1 chip            1: 2 chip
*/
#if defined(CHIP_DIE_8809E2)|| defined(CHIP_DIE_8955)
#define ADDR_SAVING_FLASHID_4_USB   0xA1A04418
#else
#define ADDR_SAVING_FLASHID_4_USB   0xA1A04410
#endif

#define FLASH_1_8MBIT    0x00000
#define FLASH_2_8MBIT    0x01000
#define FLASH_1_16MBIT   0x02000
#define FLASH_2_16MBIT   0x03000
#define FLASH_1_32MBIT   0x04000
#define FLASH_2_32MBIT   0x05000
#define FLASH_1_64MBIT   0x06000
#define FLASH_2_64MBIT   0x07000
#define FLASH_1_128MBIT  0x10000
#define FLASH_2_128MBIT  0x11000
#define FLASH_1_256MBIT  0x12000
#define FLASH_2_256MBIT  0x13000
#define FLASH_64_32MBIT  0x14000
#define FLASH_128_32MBIT 0x15000
#define FLASH_128_64MBIT 0x16000

//spi flash erase start to suspend time: n MILLI_SECONDS
#define FLASH_MICRON_ERASE2SUSPEND_TIME 0xc0
#define FLASH_GIGADEVICE_ERASE2SUSPEND_TIME 0x60
#define FLASH_COMMON_ERASE2SUSPEND_TIME 0x50


// SPI FLASH INTERFACE
#ifdef CHIP_DIE_8955
#define SPI_FLASH_REG_BASE 0xa7fff000
#else
#ifdef CHIP_DIE_8909
#define SPI_FLASH_REG_BASE 0xA1A32000
#else
#define SPI_FLASH_REG_BASE 0xa1a25000
#endif
#endif
#define CMD_ADDR_OFFSET 0x0
#define MODE_BLOCKSIZE_OFFSET 0x4
#define DATA_FIFO_OFFSET 0x8
#define STATUS_OFFSET 0xc
#define READBACK_REG_OFFSET 0x10
#define FLASH_CONFIG_OFFSET 0x14
#define FIFO_CTRL_OFFSET 0x18
#define DUAL_SPI_OFFSET 0x1c
#define READ_CMD_OFFSET 0x20
#define EXTEND_ADDR_4B_OFFSET 0x2c


#define SPI_CMD_ADDR_REG (SPI_FLASH_REG_BASE+CMD_ADDR_OFFSET)
#define SPI_FLASH_TX_COMMAND_BIT    0
#define SPI_FLASH_TX_ADDRESS_BIT    8
#define SPI_FLASH_TX_CMD_SPD_ID_BIT     0
#define SPI_FLASH_TX_COMMAND_BIT_SPD_MODE   8
#define SPI_FLASH_TX_CMD_RW_FLAG_BIT    16
#define SPI_FLASH_TX_SPI_MODE_FLAG_BIT      17
#define SPI_FLASH_CMD_ADDR_REG_RESERVED_BIT     18

#define SPI_BLOCK_SIZE_REG (SPI_FLASH_REG_BASE+MODE_BLOCKSIZE_OFFSET)
#define SPI_FLASH_MODEBIT_BIT   0
#define SPI_FLASH_RW_SIZE_BIT   8
#define SPI_FLASH_BLOCK_SIZE_REG_RESERVED_BIT   17

#define SPI_DATA_FIFO_REG (SPI_FLASH_REG_BASE+DATA_FIFO_OFFSET)
#define SPI_TX_DATA_FIFO_BIT 0
#define SPI_DATA_FIFO_REG_RESERVED_BIT  8
#define SPI_TX_DATA_FIFO_BIT_SPD_MODE     0
#define SPI_DATA_SEND_TYPE_BIT_SPD_MODE       8
#define SPI_DATA_FIFO_REG_RESERVED_BIT_SPD_MODE     8

#define SPI_STATUS_REG (SPI_FLASH_REG_BASE+STATUS_OFFSET)
#define SPI_BUSY_BIT      0
#define SPI_TX_FIFO_EMPTY_BIT   1
#define SPI_TX_FIFO_FULL_BIT     2
#define SPI_RX_FIFO_EMPTY_BIT   3
#define SPI_RX_FIFO_COUNT_BIT   4
#define SPI_STATUS_REG_RESERVED_BIT     9

#define SPI_READ_BACK_REG (SPI_FLASH_REG_BASE+READBACK_REG_OFFSET)
#define SPI_RAED_BACK_DATA_BIT 0

#define SPI_CONFIG_REG (SPI_FLASH_REG_BASE+FLASH_CONFIG_OFFSET)
#define SPI_READ_MODE_FLAG_BIT      0
#define SPI_PROTECT_PIN_BIT              1
#define SPI_HOLD_PIN_BIT                    2
#define SPI_SAMPLE_DATA_DELAY_CYCLE_BIT 4
#define SPI_CLK_DIVIDER_BIT               8
#define SPI_COMMAND_SEND_QUAD_LINES_BIT     16
#define SPI_CONFIG_REG_RESERVED_BIT             17

#define SPI_FIFO_CTRL_REG (SPI_FLASH_REG_BASE+FIFO_CTRL_OFFSET)
#define SPI_RX_FIFO_CLR_BIT         0
#define SPI_TX_FIFO_CLR_BIT         1
#define SPI_FIFO_CTRL_REG_RESERVED_BIT      2

#define SPI_CS_SIZE_REG (SPI_FLASH_REG_BASE+DUAL_SPI_OFFSET)
#define SPI_FLASH_NUMS_USED_BIT     0
#define SPI_FLASH_SIZE_USED_BIT     1
#define SPI_CS_SIZE_REG_RESERVED_BIT    2

#define SPI_READ_CMD_REG (SPI_FLASH_REG_BASE+READ_CMD_OFFSET)
#define SPI_QUAD_READ_CMD_RECFG_BIT     0
#define SPI_FAST_READ_CMD_RECFG_BIT     8
#define SPI_PROTECT_BYTE_BIT                    24


/*****************************************************************/
//  FLASH_TYPE_ID = 0 : normal flash
//  FLASH_TYPE_ID = 1 : x25u3235e
//  FLASH_TYPE_ID = 2 : en25q32
/*****************************************************************/
#define FLASH_TYPE_ID   0
#define TOTAL_FLASH_ID  3

//mx25u6435e:c2 2537
//w25q64bv:ef 4017
//w25q64dw:ef 6017
//gd25q64b:c8 4017
//en25q64:1c 3017
//en25s64:1c 3817
//s25fl064p:01 0216
//fidelix:f8 3217

typedef enum
{
    MEMD_FLASH_SPANSION = 0x01,
    MEMD_FLASH_EON = 0x1c,
    MEMD_FLASH_MXIC = 0xc2,
    MEMD_FLASH_GIGADEVICE = 0xc8,
    MEMD_FLASH_WINBOND = 0xef,
    MEMD_FLASH_FIDELIX = 0xf8,
    MEMD_FLASH_ESMT = 0x8c,
    MEMD_FLASH_XM25 = 0x20,//MEMD_FLASH_NUMONYX
    MEMD_FLASH_MICRON = 0x2c,
    MEMD_FLASH_PM25L = 0x7f,
    MEMD_FLASH_FUDAN = 0xa1
} MEMD_FLASH_MANUFACTURER_T;

typedef enum
{
    MEMD_FLASH_SPANSION_30V = 0x02,
    MEMD_FLASH_EON_30V = 0x30,
    MEMD_FLASH_EON_18V = 0x38,
    MEMD_FLASH_MXIC_30V = 0x5E,
    MEMD_FLASH_MXIC_18V = 0x25,
    MEMD_FLASH_GIGADEVICE_30V = 0x40,
    MEMD_FLASH_GIGADEVICE_18V = 0x60,
    MEMD_FLASH_WINBOND_30V = 0x40,
    MEMD_FLASH_WINBOND_18V = 0x60,
    MEMD_FLASH_FIDELIX_30V = 0x32,
    MEMD_FLASH_FIDELIX_18V = 0x42,
    MEMD_FLASH_ESMT_30V = 0x41,
    MEMD_FLASH_ESMT_18V = 0x25,
    MEMD_FLASH_NUMONYX_30V = 0xBA,
    MEMD_FLASH_MICRON_18V = 0xCB,
    MEMD_FLASH_FUDAN_30V = 0x40
} MEMD_FLASH_MEMORY_TYPE_T;

typedef enum
{
    MEMD_FLASH_SPANSION_64M = 0x16,
    MEMD_FLASH_EON_64M = 0x17,
    MEMD_FLASH_EON_32M = 0x16,
    MEMD_FLASH_MXIC_U_128M = 0x38,
    MEMD_FLASH_MXIC_U_64M = 0x37,
    MEMD_FLASH_MXIC_U_32M = 0x36,
    MEMD_FLASH_MXIC_L_32M = 0x16,
    MEMD_FLASH_GIGADEVICE_64M = 0x17,
    MEMD_FLASH_GIGADEVICE_32M = 0x16,
    MEMD_FLASH_GIGADEVICE_128M = 0x18,
    MEMD_FLASH_GIGADEVICE_256M = 0x19,
    MEMD_FLASH_WINBOND_64M = 0x17,
    MEMD_FLASH_WINBOND_32M = 0x16,
    MEMD_FLASH_FIDELIX_64M = 0x17,
    MEMD_FLASH_FIDELIX_32M = 0x16,
    MEMD_FLASH_ESMT_64M_30V = 0x17,
    MEMD_FLASH_ESMT_32M_30V = 0x16,
    MEMD_FLASH_ESMT_64M_18V = 0x37,
    MEMD_FLASH_ESMT_32M_18V = 0x36,
    MEMD_FLASH_NUMONYX_128M = 0x18,
    MEMD_FLASH_MICRON_32M = 0x16,
    MEMD_FLASH_MICRON_64M = 0x17,
    MEMD_FLASH_MICRON_128M = 0x18,
    MEMD_FLASH_FUDAN_32M = 0x16
} MEMD_FLASH_CAPACITY_TYPE_T;


typedef struct
{
    MEMD_FLASH_MANUFACTURER_T manufacturerID;
    MEMD_FLASH_MEMORY_TYPE_T device_memory_type_ID;
    MEMD_FLASH_CAPACITY_TYPE_T device_capacity_type_ID;
} MEMD_SPIFLSH_RDID_T;


#endif

///  @} <- End o flashd group

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#else
#ifdef MEMD_RAM_IS_BURST
#if !defined(RAM_WITH_DDR_MODE) || defined(__VDS_QUICK_FLUSH__)
#ifdef RAM_CLK_IS_52M
#ifdef CHIP_DIE_8955
#define PSRAM_BCR 0x08191f   // 0x08191f
#else
#define PSRAM_BCR 0x08590f   //
#endif
#else
#ifdef RAM_CLK_IS_78M
#define PSRAM_BCR 0x08710f
#else
#ifdef RAM_CLK_IS_104M
#ifdef _104M_VARIABLE_LATENCY_
#define PSRAM_BCR 0x08211f
#else
#define PSRAM_BCR 0x08710f
#endif
#else
#ifdef RAM_CLK_IS_125M
#define PSRAM_BCR 0x08410f
#else
#ifdef RAM_CLK_IS_139M
#define PSRAM_BCR 0x08410f
#error sram clk is out of 133MHz.
#else
#error sram clk is out of 133MHz.
#endif //RAM_CLK_IS_139M
#endif //RAM_CLK_IS_125M
#endif //RAM_CLK_IS_104M
#endif //RAM_CLK_IS_78M
#endif//RAM_CLK_IS_52M

#else
#ifdef RAM_CLK_IS_39M
#define PSRAM_BCR 0x08231f   //sync  0x08191f  ddr 0x8231f
#else
#ifdef RAM_CLK_IS_52M
#define PSRAM_BCR 0x086304//0x085b04
#else
#ifdef RAM_CLK_IS_78M
#define PSRAM_BCR 0x087b04//0x087304
#else
#ifdef RAM_CLK_IS_104M
#define PSRAM_BCR 0x087b04//0x087304
#else
#ifdef RAM_CLK_IS_125M
#define PSRAM_BCR 0x084304
#else
#ifdef RAM_CLK_IS_139M
#define PSRAM_BCR 0x084304
#else
#error sram clk is out of 133MHz.
#endif //RAM_CLK_IS_139M
#endif //RAM_CLK_IS_125M
#endif //RAM_CLK_IS_104M
#endif //RAM_CLK_IS_78M
#endif //RAM_CLK_IS_52M
#endif
#endif
#endif
#endif




#endif // _MEMD_M_H_
