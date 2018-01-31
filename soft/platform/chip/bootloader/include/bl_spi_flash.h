/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _BL_SPI_FLASH_H_
#define _BL_SPI_FLASH_H_

#include <stdint.h>
#include <stdbool.h>

#define SPI_FLASH_PAGE_SIZE 256
#define SPI_FLASH_SECTOR_SIZE (4 * 1024)
#define SPI_FLASH_BLOCK_SIZE (64 * 1024)
#define SPI_FLASH_BLOCK32K_SIZE (32 * 1024)

#define SPI_FLASH_OFFSET(address) ((uint32_t)(address)&0x00ffffff)
#define SPI_FLASH_UNCACHE_ADDRESS(offset) ((void *)(SPI_FLASH_OFFSET(offset) | 0xa8000000))

// =============================================================================
// bl_SpiFlashOpen
// -----------------------------------------------------------------------------
/// Initialize SPI FLASH (configure FLASH clock, read ID and etc)
// =============================================================================
void bl_SpiFlashOpen(void);

// =============================================================================
// bl_SpiFlashWrite
// -----------------------------------------------------------------------------
/// Write data to FLASH. To get correct result, it is needed to erase FLASH
/// before write.
///
/// Optionally, it can check IRQ during wait. When there are pending IRQ,
/// SUSPEND Will be called before enable IRQ, and when the task of function
/// is scheduled again, it will call RESUME.
///
/// It will return false only when parameters are invalid.
// =============================================================================
bool bl_SpiFlashWrite(uint32_t flashAddress, const void *data, unsigned size);

// =============================================================================
// bl_SpiFlashErase
// -----------------------------------------------------------------------------
/// Erase the FLASH range. The range should be sector aligned. It will return
/// false only when parameters are invalid.
// =============================================================================
bool bl_SpiFlashErase(uint32_t flashAddress, unsigned size);

// =============================================================================
// bl_SpiFlashChipErase
// -----------------------------------------------------------------------------
/// Erase the FLASH chip.
// =============================================================================
void bl_SpiFlashChipErase(void);

#endif
