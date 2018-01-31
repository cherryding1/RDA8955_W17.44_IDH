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

#include "global_macros.h"
#include "nand.h"

#include "string.h"
#include "boot.h"
#include "boot_nand.h"
#include "hal_dma.h"
#include "halp_debug.h"
#include "halp_gdb_stub.h"
#include "hal_nand.h"

#define NAND_READ_VIA_DMA 0

#define NAND_CONFIG_A_PAGE_2K   0x47b
#define NAND_CONFIG_A_PAGE_512  0x469
#define NAND_CONFIG_B           0x3e

#define NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K   11
#define NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512  9

#define NAND_PAGE_SIZE_PAGE_2K  (1<<NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K)
#define NAND_PAGE_SIZE_PAGE_512 (1<<NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512)

#define NAND_PAGE_ADDR_MASK_PAGE_2K  (NAND_PAGE_SIZE_PAGE_2K-1)
#define NAND_PAGE_ADDR_MASK_PAGE_512 (NAND_PAGE_SIZE_PAGE_512-1)

// =============================================================================
// hal_NandOpen
// -----------------------------------------------------------------------------
/// Initialize NAND flash controller
/// @param None
/// @return None
// =============================================================================
PUBLIC VOID hal_NandOpen(VOID)
{
    boot_NandOpen();
}

// =============================================================================
// hal_NandReadByPageNum
// -----------------------------------------------------------------------------
/// Read one page of data from NAND flash by page number
/// @param page    The page number in NAND flash
/// @param offset  The starting offset in the page
/// @param pData   Pointing to a buffer to hold the data (should be word-aligned)
/// @param len     The number of bytes to be read (should be mulitple of 4)
/// @return None
// =============================================================================
PRIVATE VOID hal_NandReadByPageNum(UINT32 page, UINT32 offset, UINT32 *pData, UINT32 len)
{
    // Config to indirect mode, and flush page data buffer
    hwp_nand_conf->bufcon = 7;
    // Write column address reg to 0
    hwp_nand_conf->car = 0;
    // Read page
    hwp_nand_conf->dcmd = page&0x00ffffff;
    // Wait until finished
    while(hwp_nand_conf->bsy & 0x3);
    // Page data is in buffer now

    UINT32 *bufData = (UINT32 *)&(hwp_nand_buf->data[offset>>2]);
#if (NAND_READ_VIA_DMA)
    HAL_DMA_CFG_T dmaCfg;
    dmaCfg.srcAddr = (UINT8 *)bufData;
    dmaCfg.dstAddr = (UINT8 *)pData;
    dmaCfg.transferSize = len;
    dmaCfg.mode = HAL_DMA_MODE_NORMAL;
    dmaCfg.userHandler = NULL;
    hal_DmaStart(&dmaCfg);
    while (!hal_DmaDone());
#else
    UINT32 nWords = len >> 2;
    for (UINT32 i=0; i<nWords; i++)
    {
        *pData++ = *bufData++;
    }
#endif
}


// =============================================================================
// hal_NandReadData
// -----------------------------------------------------------------------------
/// Read data from NAND flash
/// @param addr    The start address in NAND flash
/// @param pData   Pointing to a buffer to hold the data
/// @param len     The number of bytes to be read
/// @return None
// =============================================================================
PUBLIC VOID hal_NandReadData(UINT32 addr, VOID *pData, UINT32 len)
{
    UINT32 pageSize;
    UINT32 pageMask;
    UINT32 startOffset;
    UINT32 pageNum;
    UINT32 count;

    HAL_ASSERT((addr & 0x3) == 0, "hal_NandReadData: addr is not word-aligned: 0x%x", addr);
    HAL_ASSERT((len & 0x3) == 0, "hal_NandReadData: len is not word-aligned: %d", len);

    pageSize = boot_NandGetPageSize();
    if (pageSize == NAND_PAGE_SIZE_PAGE_2K)
    {
        pageMask = NAND_PAGE_ADDR_MASK_PAGE_2K;
        pageNum = addr >> NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_2K;
    }
    else
    {
        pageMask = NAND_PAGE_ADDR_MASK_PAGE_512;
        pageNum = addr >> NAND_INTRA_PAGE_ADDR_WIDTH_PAGE_512;
    }

    count = 0;
    startOffset = addr & pageMask;

    if (startOffset != 0)
    {
        UINT32 firstReadLen = pageSize - startOffset;
        hal_NandReadByPageNum(pageNum++, startOffset, pData, firstReadLen);
        pData += firstReadLen;
        count += firstReadLen;
    }

    while (count+pageSize <= len)
    {
        hal_NandReadByPageNum(pageNum++, 0, pData, pageSize);
        count += pageSize;
        pData += pageSize;
    }

    if (count < len)
    {
        hal_NandReadByPageNum(pageNum, 0, pData, len-count);
    }

#if (NAND_READ_VIA_DMA)
    hal_GdbFlushCacheData();
#endif
}


