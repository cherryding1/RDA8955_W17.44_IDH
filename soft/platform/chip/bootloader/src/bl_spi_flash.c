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

#define _STDINT_H
#define _STDBOOL_H

#include "cs_types.h"
#include "bl_spi_flash.h"
#include "bl_platform.h"
#include "global_macros.h"
#include "spi_flash.h"
#include "sys_irq.h"
#include "sys_ctrl.h"
#include "gouda.h"
#include "sys_ifc.h"
#include "dma.h"

#undef DBG_VERIFY_ERASE
#undef DBG_VERIFY_PROGRAM
#undef DBG_OP_EVENT

#ifdef DBG_OP_EVENT
#define SPI_FLASH_PROGRAM_EVENT(addr, size) bl_send_event(0x80000000 | (((addr)&0xffffff00) << 4) | ((size)&0xff))
#define SPI_FLASH_SECTOR_ERASE_EVENT(addr) bl_send_event(0x90000000 | ((addr) << 4))
#define SPI_FLASH_BLOCK_ERASE_EVENT(addr) bl_send_event(0xa0000000 | ((addr) << 4))
#define SPI_FLASH_BLOCK32_ERASE_EVENT(addr) bl_send_event(0xb0000000 | ((addr) << 4))
#else
#define SPI_FLASH_PROGRAM_EVENT(addr, size) //
#define SPI_FLASH_SECTOR_ERASE_EVENT(addr)  //
#define SPI_FLASH_BLOCK_ERASE_EVENT(addr)   //
#define SPI_FLASH_BLOCK32_ERASE_EVENT(addr) //
#endif

#define OPCODE_WRITE_ENABLE 0x06
#define OPCODE_WRITE_DISABLE 0x04
#define OPCODE_READ_STATUS 0x05
#define OPCODE_READ_STATUS_2 0x35
#define OPCODE_WRITE_STATUS 0x01
#define OPCODE_SECTOR_ERASE 0x20
#define OPCODE_BLOCK_ERASE 0xd8
#define OPCODE_BLOCK32K_ERASE 0x52
#define OPCODE_CHIP_ERASE 0xc7 // or 0x60
#define OPCODE_HIGH_PERF_MODE gSpiFlashCtx.opcodeHighPerfMode
#define OPCODE_RESET_ENABLE 0x66
#define OPCODE_RESET 0x99
#define OPCODE_PAGE_PROGRAM 0x02
#define OPCODE_READ_ID 0x9f

#define DELAY_TRST 100 // us, 30us in datasheet

#define STREG1_WIP (1 << 0)
#define STREG2_QE (1 << 1)

#define SPI_NORMAL_CMD(cmd, address) (((address) << 8) | ((cmd)&0xff))
#define SPI_TX_CMD(cmd) (((cmd)&0xff) << 8)
#define SPI_RX_SCMD(cmd) ((((cmd)&0xff) | (1 << 8) | (0 << 9)) << 8)
#define SPI_RX_QCMD(cmd) ((((cmd)&0xff) | (1 << 8) | (1 << 9)) << 8)
#define SPI_SET_BLOCK_SIZE(n) hwp_spiFlash->spi_block_size = (hwp_spiFlash->spi_block_size & ~0x1ff00) | ((n) << 8)
#define SPI_SET_RX_TX_SIZE(n) hwp_spiFlash->spi_config = (hwp_spiFlash->spi_config & ~(3 << 17)) | ((n) << 17)

#define SPI_FLASH_PAGE_MASK (SPI_FLASH_PAGE_SIZE - 1)
#define SPI_FLASH_SECTOR_MASK (SPI_FLASH_SECTOR_SIZE - 1)
#define SPI_FLASH_BLOCK_MASK (SPI_FLASH_BLOCK_SIZE - 1)
#define SPI_FLASH_BLOCK32K_MASK (SPI_FLASH_BLOCK32K_SIZE - 1)

#ifdef CHIP_DIE_8809E2
#define SPI_RX_USE_FIFO
#else
#define SPI_RX_USE_READBACK
#endif

#define DELAY_BEFORE_CHECK_WIP 20 // us

#ifdef CHIP_DIE_8909 // same register with different name
#define spi_data_fifo_wo spi_data_fifo
#define spi_data_fifo_ro spi_fifo_status
#define hwp_dma hwp_sysDma
#endif

typedef enum {
    MEMD_FLASH_GIGADEVICE = 0xc8,
    MEMD_FLASH_WINBOND = 0xef,
    MEMD_FLASH_XM25 = 0x20
} MEMD_FLASH_MANUFACTURER_T;

typedef struct
{
    uint32_t id;
    uint8_t manufacturerID;
    uint8_t memoryTypeID;
    uint8_t capacityID;
    uint8_t opcodeHighPerfMode;
} HAL_SPI_FLASH_CONTEXT_T;

static HAL_SPI_FLASH_CONTEXT_T gSpiFlashCtx;

static void bl_SpiFlashWaitNotBusy(void)
{
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_SPI_FLASH_BUSY)
        ;
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_SPI_FLASH_BUSY)
        ;
}

// maybe unused if not defined `SPI_RX_USE_FIFO`
static __attribute__((unused)) void bl_SpiFlashWaitRxFifo(uint8_t cnt)
{
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_RX_FIFO_EMPTY)
        ;
    while (((hwp_spiFlash->spi_data_fifo_ro >> 4) & 31) < cnt)
        ;
}

static void bl_SpiSendCmdNoRx(uint8_t cmd, const uint8_t *data, unsigned size)
{
    if (cmd == 0)
        return;

    bl_SpiFlashWaitNotBusy();
    hwp_spiFlash->spi_fifo_control = SPI_FLASH_TX_FIFO_CLR;
    for (unsigned n = 0; n < size; n++)
        hwp_spiFlash->spi_data_fifo_wo = data[n];
    hwp_spiFlash->spi_cmd_addr = SPI_TX_CMD(cmd);
}

static uint32_t bl_SpiCmdWithRx(uint32_t cmdReg, const uint8_t *txdata, unsigned txsize, unsigned rxsize, unsigned checkcount)
{
    uint32_t prev = 0;
    int count = 0;

    while (count <= checkcount)
    {
        bl_SpiFlashWaitNotBusy();
        hwp_spiFlash->spi_fifo_control = (SPI_FLASH_TX_FIFO_CLR | SPI_FLASH_RX_FIFO_CLR);
        SPI_SET_BLOCK_SIZE(rxsize);

#ifdef SPI_RX_USE_READBACK
        SPI_SET_RX_TX_SIZE(rxsize == 1 ? 0 : rxsize == 2 ? 1 : rxsize == 3 ? 3 : 2);
#endif

        for (unsigned n = 0; n < txsize; n++)
            hwp_spiFlash->spi_data_fifo_wo = txdata[n];
        hwp_spiFlash->spi_cmd_addr = cmdReg;
        bl_SpiFlashWaitNotBusy();

#ifdef SPI_RX_USE_FIFO
        bl_SpiFlashWaitRxFifo(rxsize);
        uint32_t result = hwp_spiFlash->spi_data_fifo_wo & 0xff;
        if (rxsize > 1)
            result |= (hwp_spiFlash->spi_data_fifo_wo & 0xff) << 8;
        if (rxsize > 2)
            result |= (hwp_spiFlash->spi_data_fifo_wo & 0xff) << 16;
        if (rxsize > 3)
            result |= (hwp_spiFlash->spi_data_fifo_wo & 0xff) << 24;
#endif

#ifdef SPI_RX_USE_READBACK
        uint32_t result = hwp_spiFlash->spi_read_back >> ((4 - rxsize) * 8);
        SPI_SET_RX_TX_SIZE(0);
#endif

        SPI_SET_BLOCK_SIZE(1);
        count = (count == 0 || result != prev) ? 1 : (count + 1);
        prev = result;
    }
    return prev;
}

static inline void bl_SpiCmdWriteEnable(void)
{
    uint32_t id = bl_SpiCmdWithRx(OPCODE_READ_ID, NULL, 0, 3, 3);
    if (id != gSpiFlashCtx.id)
        bl_panic();
    bl_SpiSendCmdNoRx(OPCODE_WRITE_ENABLE, NULL, 0);
}

static inline void bl_SpiCmdWriteDisable(void)
{
    bl_SpiSendCmdNoRx(OPCODE_WRITE_DISABLE, NULL, 0);
}

static inline void bl_SpiFlashWriteStatus(uint8_t lo, uint8_t hi)
{
    uint8_t data[2] = {lo, hi};
    bl_SpiSendCmdNoRx(OPCODE_WRITE_STATUS, data, 2);
}

static inline void bl_SpiCmdEraseWithOpcode(uint8_t cmd, uint32_t address)
{
    uint8_t data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, (address >> 0) & 0xff};
    bl_SpiSendCmdNoRx(cmd, data, 3);
}

static inline void bl_SpiCmdChipErase(void)
{
    bl_SpiSendCmdNoRx(OPCODE_CHIP_ERASE, NULL, 0);
}

static void bl_SpiCmdProgram(uint32_t address, const void *data, unsigned size)
{
    bl_SpiFlashWaitNotBusy();
    hwp_spiFlash->spi_fifo_control = SPI_FLASH_TX_FIFO_CLR;
    for (unsigned n = 0; n < size; n++)
        hwp_spiFlash->spi_data_fifo_wo = ((const uint8_t *)data)[n];
    hwp_spiFlash->spi_cmd_addr = SPI_NORMAL_CMD(OPCODE_PAGE_PROGRAM, address);
}

static inline void bl_SpiCmdEnableReset(void)
{
    bl_SpiSendCmdNoRx(OPCODE_RESET_ENABLE, NULL, 0);
}

static inline void bl_SpiCmdReset(void)
{
    bl_SpiSendCmdNoRx(OPCODE_RESET, NULL, 0);
}

static inline void bl_SpiCmdHighPerfMode(void)
{
    uint8_t data[3] = {0, 0, 0};
    bl_SpiSendCmdNoRx(OPCODE_HIGH_PERF_MODE, data, 3);
}

static inline uint8_t bl_SpiFlashReadStatus(void)
{
    // repeat 3 times for safety
    return bl_SpiCmdWithRx(OPCODE_READ_STATUS, NULL, 0, 1, 3) & 0xff;
}

static inline uint8_t bl_SpiFlashReadStatus2(void)
{
    // repeat 3 times for safety
    return bl_SpiCmdWithRx(OPCODE_READ_STATUS_2, NULL, 0, 1, 3) & 0xff;
}

static void bl_SpiFlashWaitWip(void)
{
    bl_delay_us(DELAY_BEFORE_CHECK_WIP);
    while (bl_SpiFlashReadStatus() & STREG1_WIP)
        ;
}

static void bl_SpiFlashEraseWithOpcode(uint8_t cmd, uint32_t flashAddress)
{
    bl_SpiCmdWriteEnable();
    bl_SpiCmdEraseWithOpcode(cmd, flashAddress);
    bl_SpiFlashWaitWip();
    bl_SpiCmdHighPerfMode();
    bl_SpiCmdWriteDisable();
}

void bl_SpiFlashSectorErase(uint32_t flashAddress)
{
    SPI_FLASH_SECTOR_ERASE_EVENT(flashAddress);
    bl_SpiFlashEraseWithOpcode(OPCODE_SECTOR_ERASE, flashAddress);
}

void bl_SpiFlashBlockErase(uint32_t flashAddress)
{
    SPI_FLASH_BLOCK_ERASE_EVENT(flashAddress);
    bl_SpiFlashEraseWithOpcode(OPCODE_BLOCK_ERASE, flashAddress);
}

void bl_SpiFlashBlock32KErase(uint32_t flashAddress)
{
    SPI_FLASH_BLOCK32_ERASE_EVENT(flashAddress);
    bl_SpiFlashEraseWithOpcode(OPCODE_BLOCK32K_ERASE, flashAddress);
}

void bl_SpiFlashProgram(uint32_t flashAddress, const void *data, unsigned size)
{
    uint8_t ramdata[SPI_FLASH_PAGE_SIZE];
    memcpy(ramdata, data, size);

    SPI_FLASH_PROGRAM_EVENT(flashAddress, size);

    bl_SpiCmdWriteEnable();
    bl_SpiCmdProgram(flashAddress, ramdata, size);
    bl_SpiFlashWaitWip();
    bl_SpiCmdHighPerfMode();
    bl_SpiCmdWriteDisable();
}

void bl_SpiFlashOpen(void)
{
#if defined(CHIP_DIE_8909)
    hwp_sysCtrl->cfg_clk_spiflash = SYS_CTRL_SPIFLASH_FREQ_56MHZ; // TODO: 8909
#else
    hwp_sysCtrl->Cfg_Clk_SpiFlash = SYS_CTRL_SPIFLASH_FREQ_156M;
#endif

    hwp_spiFlash->spi_config = SPI_FLASH_SAMPLE_DELAY(1) |
                               SPI_FLASH_CLK_DIVIDER(2) |
                               SPI_FLASH_BYPASS_START_CMD |
                               SPI_FLASH_QUAD_MODE_QUAD_READ |
                               0 /* SPI_FLASH_SPI_WPROTECT_PIN */ |
                               0 /* SPI_FLASH_SPI_HOLD_PIN */ |
                               0 /* SPI_FLASH_CMD_QUAD */ |
                               SPI_FLASH_TX_RX_SIZE(0);

    bl_SpiCmdEnableReset();
    bl_SpiCmdReset();
    bl_delay_us(DELAY_TRST);

    uint32_t id = bl_SpiCmdWithRx(OPCODE_READ_ID, NULL, 0, 3, 3);
    bl_send_event(id);

    gSpiFlashCtx.id = id;
    gSpiFlashCtx.manufacturerID = (id & 0xff);
    gSpiFlashCtx.memoryTypeID = ((id >> 8) & 0xff);
    gSpiFlashCtx.capacityID = ((id >> 16) & 0xff);

    gSpiFlashCtx.opcodeHighPerfMode = 0;
    if (gSpiFlashCtx.manufacturerID == MEMD_FLASH_WINBOND)
        gSpiFlashCtx.opcodeHighPerfMode = 0xa3;

    uint8_t sr2 = bl_SpiFlashReadStatus2();
    if ((sr2 & STREG2_QE) == 0)
    {
        sr2 |= STREG2_QE;
        uint8_t sr1 = bl_SpiFlashReadStatus();
        bl_SpiCmdWriteEnable();
        bl_SpiFlashWriteStatus(sr1, sr2);
        bl_SpiFlashWaitWip();
        bl_SpiCmdWriteDisable();
    }
}

unsigned bl_SpiFlashGetSize(void) { return 1 << gSpiFlashCtx.capacityID; }

bool bl_SpiFlashWrite(uint32_t flashAddress, const void *data, unsigned size)
{
    uint32_t capacity = (1 << gSpiFlashCtx.capacityID);
    if (flashAddress >= capacity ||
        (flashAddress + size) > capacity ||
        data == NULL)
        return false;

#ifdef DBG_VERIFY_PROGRAM
    unsigned total = size;
#endif
    while (size > 0)
    {
        unsigned nextPage = (flashAddress + SPI_FLASH_PAGE_SIZE) & ~SPI_FLASH_PAGE_MASK;
        unsigned trans = nextPage - flashAddress;
        if (trans > size)
            trans = size;

        bl_SpiFlashProgram(flashAddress, data, trans);
        flashAddress += trans;
        data += trans;
        size -= trans;
    }

#ifdef DBG_VERIFY_PROGRAM
    const uint8_t *fp = SPI_FLASH_UNCACHE_ADDRESS(flashAddress - total);
    const uint8_t *fd = (const uint8_t *)data - total;
    if (memcmp(fp, fd, total) != 0)
        asm("break 1");
#endif

    return true;
}

bool bl_SpiFlashErase(uint32_t flashAddress, unsigned size)
{
    uint32_t capacity = (1 << gSpiFlashCtx.capacityID);
    if (flashAddress >= capacity ||
        (flashAddress + size) > capacity ||
        (flashAddress & SPI_FLASH_SECTOR_MASK) != 0 ||
        (size & SPI_FLASH_SECTOR_MASK) != 0)
        return false;

#ifdef DBG_VERIFY_ERASE
    unsigned total = size;
#endif
    while (size > 0)
    {
        if ((flashAddress & SPI_FLASH_BLOCK_MASK) == 0 && size >= SPI_FLASH_BLOCK_SIZE)
        {
            bl_SpiFlashBlockErase(flashAddress);
            flashAddress += SPI_FLASH_BLOCK_SIZE;
            size -= SPI_FLASH_BLOCK_SIZE;
        }
        else if ((flashAddress & SPI_FLASH_BLOCK32K_MASK) == 0 && size >= SPI_FLASH_BLOCK32K_SIZE)
        {
            bl_SpiFlashBlock32KErase(flashAddress);
            flashAddress += SPI_FLASH_BLOCK32K_SIZE;
            size -= SPI_FLASH_BLOCK32K_SIZE;
        }
        else
        {
            bl_SpiFlashSectorErase(flashAddress);
            flashAddress += SPI_FLASH_SECTOR_SIZE;
            size -= SPI_FLASH_SECTOR_SIZE;
        }
    }

#ifdef DBG_VERIFY_ERASE
    const uint8_t *fp = SPI_FLASH_UNCACHE_ADDRESS(flashAddress - total);
    for (unsigned n = 0; n < total; n++)
    {
        if (fp[n] != 0xff)
            asm("break 1");
    }
#endif

    return true;
}

void bl_SpiFlashChipErase(void)
{
    bl_SpiCmdWriteEnable();
    bl_SpiCmdChipErase();
    bl_SpiFlashWaitWip();
    bl_SpiCmdWriteDisable();
}
