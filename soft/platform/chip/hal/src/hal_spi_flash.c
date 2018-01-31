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

#include "hal_spi_flash.h"
#include "hal_sys.h"
#include "hal_gouda.h"
#include "global_macros.h"
#include "spi_flash.h"
#include "sys_irq.h"
#include "sys_ctrl.h"
#include "gouda.h"
#include "sys_ifc.h"
#include "dma.h"
#include "voc_ahb.h"
#include "reg_alias.h"
#include "hal_mem_map.h"
#include "string.h"
// External dependency, can't located on FLASH
// * hal_SysEnterCriticalSection
// * hal_SysExitCriticalSection
// * hal_SysWaitMicrosecond
// * mon_Event
// * sxr_GetCurrentTaskId (when DBG_OP_TASK_EVENT is defined)

#undef DBG_VERIFY_ERASE
#undef DBG_VERIFY_PROGRAM
#undef DBG_OP_EVENT
#undef DBG_OP_TICK_EVENT
#undef DBG_OP_TASK_EVENT
#define DBG_CHECK_VOC_DMA
#define DBG_TOGGLE_AHB_READ
#define DBG_ASSERT_WIP_NONSTABLE

#ifdef DBG_ASSERT_WIP_NONSTABLE
#define SPI_WIP_ASSERT() asm("break 1")
#else
#define SPI_WIP_ASSERT()
#endif

#ifdef DBG_OP_EVENT
#define SPI_OP_EVENT(evt) mon_Event(evt)
#else
#define SPI_OP_EVENT(evt)
#endif

#ifdef DBG_OP_TICK_EVENT
#define SPI_FLASH_OP_START() unsigned op_tick = hal_TimGetUpTime()
#define SPI_FLASH_OP_END() mon_Event(0xf1f00000 | (hal_TimGetUpTime() - op_tick))
#else
#define SPI_FLASH_OP_START()
#define SPI_FLASH_OP_END()
#endif

#ifdef DBG_OP_TASK_EVENT
#define SPI_FLASH_OP_TASK() mon_Event(0xf1e00000 | sxr_GetCurrentTaskId())
#else
#define SPI_FLASH_OP_TASK()
#endif

#define SPI_FLASH_SECTOR_ERASE_EVENT(addr) SPI_OP_EVENT(0xf1100000 | ((addr) / SPI_FLASH_SECTOR_SIZE))
#define SPI_FLASH_BLOCK_ERASE_EVENT(addr) SPI_OP_EVENT(0xf1200000 | ((addr) / SPI_FLASH_SECTOR_SIZE))
#define SPI_FLASH_BLOCK32K_ERASE_EVENT(addr) SPI_OP_EVENT(0xf1300000 | ((addr) / SPI_FLASH_SECTOR_SIZE))
#define SPI_FLASH_PROGRAM_EVENT(addr) SPI_OP_EVENT(0xf1400000 | ((addr) / SPI_FLASH_SECTOR_SIZE))
#define SPI_FLASH_PROGRAM_SIZE_EVENT(addr, size) SPI_OP_EVENT(0xf1500000 | (((addr)&SPI_FLASH_SECTOR_MASK) << 8) | ((size)&0xff))

#define OPCODE_WRITE_ENABLE 0x06
#define OPCODE_WRITE_VOLATILE_STATUS_ENABLE 0x50
#define OPCODE_WRITE_DISABLE 0x04
#define OPCODE_READ_STATUS 0x05
#define OPCODE_READ_STATUS_2 0x35
#define OPCODE_READ_STATUS_3 0x15
#define OPCODE_WRITE_STATUS 0x01
#define OPCODE_WRITE_STATUS_2 0x31
#define OPCODE_WRITE_STATUS_3 0x11
#define OPCODE_SECTOR_ERASE 0x20
#define OPCODE_BLOCK_ERASE 0xd8
#define OPCODE_BLOCK32K_ERASE 0x52
#define OPCODE_CHIP_ERASE 0xc7 // or 0x60
#define OPCODE_ERASE_SUSPEND 0x75
#define OPCODE_ERASE_RESUME 0x7a
#define OPCODE_PROGRAM_SUSPEND 0x75
#define OPCODE_PROGRAM_RESUME 0x7a
#define OPCODE_HIGH_PERF_MODE gSpiFlashCtx.opcodeHighPerfMode
#define OPCODE_RESET_ENABLE 0x66
#define OPCODE_RESET 0x99
#define OPCODE_PAGE_PROGRAM 0x02
#define OPCODE_READ_ID 0x9f
#define OPCODE_SECURITY_ERASE 0x44
#define OPCODE_SECURITY_PROGRAM 0x42
#define OPCODE_SECURITY_READ 0x48
#define OPCODE_POWER_DOWN 0xb9
#define OPCODE_RELEASE_POWER_DOWN 0xab

#define DELAY_TRES1 50 // us, 20us in datasheet
#define DELAY_TRST 100 // us, 30us in datasheet

#define STREG1_WIP (1 << 0)
#define STREG2_QE (1 << 1)

#define SPI_NORMAL_CMD(cmd, address) (((address) << 8) | ((cmd)&0xff))
#define SPI_TX_CMD(cmd) (((cmd)&0xff) << 8)
#define SPI_RX_SCMD(cmd) ((((cmd)&0xff) | (1 << 8) | (0 << 9)) << 8)
#define SPI_RX_QCMD(cmd) ((((cmd)&0xff) | (1 << 8) | (1 << 9)) << 8)
#define SPI_SET_BLOCK_SIZE(n) hwp_spiFlash->spi_block_size = (hwp_spiFlash->spi_block_size & ~0x1ff00) | ((n) << 8)
#define SPI_SET_RX_TX_SIZE(n) hwp_spiFlash->spi_config = (hwp_spiFlash->spi_config & ~(3 << 17)) | ((n) << 17)

#define SPI_FLASH_PAGE_SIZE 256
#define SPI_FLASH_SECTOR_SIZE (4 * 1024)
#define SPI_FLASH_BLOCK_SIZE (64 * 1024)
#define SPI_FLASH_BLOCK32K_SIZE (32 * 1024)

#define SPI_FLASH_PAGE_MASK (SPI_FLASH_PAGE_SIZE - 1)
#define SPI_FLASH_SECTOR_MASK (SPI_FLASH_SECTOR_SIZE - 1)
#define SPI_FLASH_BLOCK_MASK (SPI_FLASH_BLOCK_SIZE - 1)
#define SPI_FLASH_BLOCK32K_MASK (SPI_FLASH_BLOCK32K_SIZE - 1)

#ifdef CHIP_DIE_8809E2
#define SPI_RX_USE_FIFO
#else
#define SPI_RX_USE_READBACK
#endif

#define PHYADDR_IN_FLASH(address) (((address)&0x0f000000) == 0x08000000)
#define NOP_FOR_EXIT_SC() asm("nop;nop;nop;nop")
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
    bool checkirq;
    uint32_t id;
    uint8_t manufacturerID;
    uint8_t memoryTypeID;
    uint8_t capacityID;
    uint8_t opcodeHighPerfMode;
    HAL_CALLBACK_T lock;
    HAL_CALLBACK_T unlock;
} HAL_SPI_FLASH_CONTEXT_T;

static HAL_SPI_FLASH_CONTEXT_T HAL_BOOT_DATA_INTERNAL gSpiFlashCtx;

static void hal_SpiFlashWaitNotBusy(void)
{
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_SPI_FLASH_BUSY)
        ;
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_SPI_FLASH_BUSY)
        ;
}

static void hal_SpiFlashWaitRxFifo(uint8_t cnt)
{
    while (hwp_spiFlash->spi_data_fifo_ro & SPI_FLASH_RX_FIFO_EMPTY)
        ;
    while (((hwp_spiFlash->spi_data_fifo_ro >> 4) & 31) < cnt)
        ;
}

static void hal_SpiFlashEnableAhbRead(bool enabled)
{
#ifdef DBG_TOGGLE_AHB_READ
    hal_SpiFlashWaitNotBusy();
    uint32_t reg = hwp_spiFlash->spi_cs_size;
    if (enabled)
        reg &= ~SPI_FLASH_AHB_READ_DISABLE_DISABLE_AHB_READ;
    else
        reg |= SPI_FLASH_AHB_READ_DISABLE_DISABLE_AHB_READ;
    hwp_spiFlash->spi_cs_size = reg;
#endif
}

static void hal_SpiSendCmdNoRx(uint8_t cmd, const uint8_t *data, unsigned size)
{
    if (cmd == 0)
        return;

    hal_SpiFlashWaitNotBusy();
    hwp_spiFlash->spi_fifo_control = SPI_FLASH_TX_FIFO_CLR;
    hal_SpiFlashWaitNotBusy();
    for (unsigned n = 0; n < size; n++)
        hwp_spiFlash->spi_data_fifo_wo = data[n];
    hwp_spiFlash->spi_cmd_addr = SPI_TX_CMD(cmd);
    hal_SpiFlashWaitNotBusy();
}

static void hal_SpiSendCmdNormalNoRx(uint8_t cmd, uint32_t address, const uint8_t *data, unsigned size)
{
    if (cmd == 0)
        return;

    hal_SpiFlashWaitNotBusy();
    hwp_spiFlash->spi_fifo_control = SPI_FLASH_TX_FIFO_CLR;
    hal_SpiFlashWaitNotBusy();
    for (unsigned n = 0; n < size; n++)
        hwp_spiFlash->spi_data_fifo_wo = data[n];
    hwp_spiFlash->spi_cmd_addr = SPI_NORMAL_CMD(cmd, address);
    hal_SpiFlashWaitNotBusy();
}

static uint32_t hal_SpiCmdWithRx(uint8_t cmd, bool qread, const uint8_t *txdata, unsigned txsize, unsigned rxsize)
{
    hal_SpiFlashWaitNotBusy();
    hwp_spiFlash->spi_fifo_control = (SPI_FLASH_TX_FIFO_CLR | SPI_FLASH_RX_FIFO_CLR);

    hal_SpiFlashWaitNotBusy();
    SPI_SET_BLOCK_SIZE(rxsize);

#ifdef SPI_RX_USE_READBACK
    SPI_SET_RX_TX_SIZE(rxsize == 1 ? 0 : rxsize == 2 ? 1 : rxsize == 3 ? 3 : 2);
#endif

    hal_SpiFlashWaitNotBusy();
    for (unsigned n = 0; n < txsize; n++)
        hwp_spiFlash->spi_data_fifo_wo = txdata[n];
    hwp_spiFlash->spi_cmd_addr = qread ? SPI_RX_QCMD(cmd) : SPI_RX_SCMD(cmd);
    hal_SpiFlashWaitNotBusy();

#ifdef SPI_RX_USE_FIFO
    hal_SpiFlashWaitRxFifo(rxsize);
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
    return result;
}

static inline void hal_SpiCmdWriteEnable(void)
{
    uint32_t id = hal_SpiCmdWithRx(OPCODE_READ_ID, false, NULL, 0, 3);
    if (id != gSpiFlashCtx.id)
        asm("break 1");
    hal_SpiSendCmdNoRx(OPCODE_WRITE_ENABLE, NULL, 0);
}

static inline void hal_SpiCmdWriteDisable(void)
{
    hal_SpiSendCmdNoRx(OPCODE_WRITE_DISABLE, NULL, 0);
}

static inline void hal_SpiCmdWriteVolatileEnable(void)
{
    hal_SpiSendCmdNoRx(OPCODE_WRITE_VOLATILE_STATUS_ENABLE, NULL, 0);
}

static inline void hal_SpiFlashWriteStatus(uint8_t lo, uint8_t hi)
{
    uint8_t data[2] = {lo, hi};
    hal_SpiSendCmdNoRx(OPCODE_WRITE_STATUS, data, 2);
}

static inline void hal_SpiFlashWriteStatusEx(uint8_t n, uint8_t val)
{
    uint8_t cmd = (n == 1) ? OPCODE_WRITE_STATUS : (n == 2) ? OPCODE_WRITE_STATUS_2 : (n == 3) ? OPCODE_WRITE_STATUS_3 : 0;
    hal_SpiSendCmdNoRx(cmd, &val, 1);
}

static inline void hal_SpiCmdEraseWithOpcode(uint8_t cmd, uint32_t address)
{
    uint8_t data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, (address >> 0) & 0xff};
    hal_SpiSendCmdNoRx(cmd, data, 3);
}

static inline void hal_SpiCmdChipErase(void)
{
    hal_SpiSendCmdNoRx(OPCODE_CHIP_ERASE, NULL, 0);
}

static void hal_SpiCmdProgram(uint32_t address, const uint8_t *data, unsigned size)
{
    hal_SpiSendCmdNormalNoRx(OPCODE_PAGE_PROGRAM, address, data, size);
}

static inline void hal_SpiCmdEnableReset(void)
{
    hal_SpiSendCmdNoRx(OPCODE_RESET_ENABLE, NULL, 0);
}

static inline void hal_SpiCmdReset(void)
{
    hal_SpiSendCmdNoRx(OPCODE_RESET, NULL, 0);
}

static inline void hal_SpiCmdEraseSuspend(void)
{
    hal_SpiSendCmdNoRx(OPCODE_ERASE_SUSPEND, NULL, 0);
}

static inline void hal_SpiCmdEraseResume(void)
{
    hal_SpiSendCmdNoRx(OPCODE_ERASE_RESUME, NULL, 0);
}

static inline void hal_SpiCmdProgramSuspend(void)
{
    hal_SpiSendCmdNoRx(OPCODE_PROGRAM_SUSPEND, NULL, 0);
}

static inline void hal_SpiCmdProgramResume(void)
{
    hal_SpiSendCmdNoRx(OPCODE_PROGRAM_RESUME, NULL, 0);
}

static inline void hal_SpiCmdReleasePowerDown(void)
{
    hal_SpiSendCmdNoRx(OPCODE_RELEASE_POWER_DOWN, NULL, 0);
}

static inline void hal_SpiCmdPowerDown(void)
{
    hal_SpiSendCmdNoRx(OPCODE_POWER_DOWN, NULL, 0);
}

static inline void hal_SpiCmdHighPerfMode(void)
{
    uint8_t data[3] = {0, 0, 0};
    hal_SpiSendCmdNoRx(OPCODE_HIGH_PERF_MODE, data, 3);
}

static inline void hal_SpiCmdSecurityErase(uint32_t address)
{
    uint8_t data[3] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff};
    hal_SpiSendCmdNoRx(OPCODE_SECURITY_ERASE, data, 3);
}

static void hal_SpiCmdSecurityProgram(uint32_t address, const uint8_t *data, uint8_t size)
{
    hal_SpiSendCmdNormalNoRx(OPCODE_SECURITY_PROGRAM, address, data, size);
}

static uint32_t hal_SpiCmdSecurityRead(uint32_t address, unsigned size)
{
    uint8_t data[4] = {(address >> 16) & 0xff, (address >> 8) & 0xff, address & 0xff, 0};
    return hal_SpiCmdWithRx(OPCODE_SECURITY_READ, false, data, 4, size);
}

static inline uint8_t hal_SpiFlashReadStatus(void)
{
    return hal_SpiCmdWithRx(OPCODE_READ_STATUS, false, NULL, 0, 1) & 0xff;
}

static inline uint8_t hal_SpiFlashReadStatus2(void)
{
    return hal_SpiCmdWithRx(OPCODE_READ_STATUS_2, false, NULL, 0, 1) & 0xff;
}

static inline uint8_t hal_SpiFlashReadStatus3(void)
{
    return hal_SpiCmdWithRx(OPCODE_READ_STATUS_3, false, NULL, 0, 1) & 0xff;
}

static uint32_t hal_SpiFlashWaitWipDoneLocked(uint32_t sc)
{
    for (;;)
    {
        hal_SysWaitMicrosecond(DELAY_BEFORE_CHECK_WIP);
        if (hal_SpiFlashReadStatus() & STREG1_WIP)
        {
            hal_SysExitCriticalSection(sc);
            NOP_FOR_EXIT_SC();
            sc = hal_SysEnterCriticalSection();
            continue;
        }

        if (hal_SpiFlashReadStatus() & STREG1_WIP)
        {
            SPI_WIP_ASSERT();
            continue;
        }
        return sc;
    }
}

static void hal_SpiFlashWaitWipDone(void)
{
    for (;;)
    {
        hal_SysWaitMicrosecond(DELAY_BEFORE_CHECK_WIP);
        if (hal_SpiFlashReadStatus() & STREG1_WIP)
            continue;
        if (hal_SpiFlashReadStatus() & STREG1_WIP)
        {
            SPI_WIP_ASSERT();
            continue;
        }
        break;
    }
}

static bool hal_SpiFlashCheckWipDone(void)
{
    hal_SysWaitMicrosecond(DELAY_BEFORE_CHECK_WIP);
    if (hal_SpiFlashReadStatus() & STREG1_WIP)
        return false;
    if (hal_SpiFlashReadStatus() & STREG1_WIP)
    {
        SPI_WIP_ASSERT();
        return false;
    }
    return true;
}

static inline bool hal_SpiFlashOtherMaster(void)
{
    if ((hwp_dma->status & DMA_ENABLE) == DMA_ENABLE &&
        (hwp_dma->status & DMA_INT_DONE_STATUS) != DMA_INT_DONE_STATUS &&
        PHYADDR_IN_FLASH(hwp_dma->src_addr))
        return true;

    for (int i = 0; i < SYS_IFC_STD_CHAN_NB; i++)
    {
        if ((hwp_sysIfc->std_ch[i].status & SYS_IFC_ENABLE) == SYS_IFC_ENABLE &&
            (hwp_sysIfc->std_ch[i].tc) != 0 &&
            PHYADDR_IN_FLASH(hwp_sysIfc->std_ch[i].start_addr))
            return true;
    }

    if ((hwp_gouda->gd_status & (GOUDA_IA_BUSY | GOUDA_LCD_BUSY)) != 0)
    {
        for (int id = 0; id < HAL_GOUDA_OVL_LAYER_ID_QTY; id++)
        {
            if (PHYADDR_IN_FLASH(hwp_gouda->Overlay_Layer[id].gd_ol_rgb_src) &&
                (hwp_gouda->Overlay_Layer[id].gd_ol_input_fmt & GOUDA_ACTIVE) == GOUDA_ACTIVE)
                return true;
        }

        if ((PHYADDR_IN_FLASH(hwp_gouda->gd_vl_y_src) ||
             PHYADDR_IN_FLASH(hwp_gouda->gd_vl_u_src) ||
             PHYADDR_IN_FLASH(hwp_gouda->gd_vl_v_src)) &&
            (hwp_gouda->gd_vl_input_fmt & GOUDA_ACTIVE) == GOUDA_ACTIVE)
            return true;
    }
    // check whether voc is working, for the voc start dma itself.
    // if ((hwp_sysCtrl->BB_Rst_Set & SYS_CTRL_SET_RST_VOC) == SYS_CTRL_SET_RST_VOC)
		//return true
    return false;
}

static inline void hal_SpiFlashCheckVoc(void)
{
#ifdef DBG_CHECK_VOC_DMA
    if ((hwp_sysCtrl->BB_Rst_Set & SYS_CTRL_SET_RST_VOC) == SYS_CTRL_SET_RST_VOC &&
        (hwp_vocAhb->DMA_Status & VOC_AHB_DMA_ON) == VOC_AHB_DMA_ON &&
        PHYADDR_IN_FLASH(hwp_vocAhb->DMA_EAddr))
        asm("break 1");
#endif
}

static uint32_t hal_SpiFlashWaitOtherMaster(void)
{
    uint32_t sc = hal_SysEnterCriticalSection();
    while (hal_SpiFlashOtherMaster())
    {
        hal_SysExitCriticalSection(sc);
        NOP_FOR_EXIT_SC();
        sc = hal_SysEnterCriticalSection();
    }
    return sc;
}

static void hal_SpiFlashEraseWithOpcode(uint8_t cmd, uint32_t flashAddress)
{
    uint32_t sc = hal_SpiFlashWaitOtherMaster();

    SPI_FLASH_OP_TASK();
    bool checkirq = gSpiFlashCtx.checkirq;
    if (sc == 0 || OPCODE_ERASE_SUSPEND == 0 || OPCODE_ERASE_RESUME == 0)
        checkirq = false;

    hal_SpiFlashEnableAhbRead(false);
    hal_SpiCmdWriteEnable();
    hal_SpiCmdEraseWithOpcode(cmd, flashAddress);

    for (;;)
    {
        hal_SpiFlashCheckVoc();
        if (hal_SpiFlashCheckWipDone())
        {
            hal_SpiCmdHighPerfMode();
            hal_SpiCmdWriteDisable();
            hal_SpiFlashEnableAhbRead(true);
            hal_SysExitCriticalSection(sc);
            return;
        }

        if (checkirq)
        {
            if (hwp_sysIrq->Cause != 0)
            {
                hal_SpiCmdEraseSuspend();
                hal_SpiFlashWaitWipDone();
                hal_SpiCmdHighPerfMode();
                hal_SpiFlashEnableAhbRead(true);
                hal_SysExitCriticalSection(sc);

                NOP_FOR_EXIT_SC();
                sc = hal_SpiFlashWaitOtherMaster();
                hal_SpiFlashEnableAhbRead(false);
                hal_SpiCmdEraseResume();
            }
        }
    }
}

void hal_SpiFlashSectorErase(uint32_t flashAddress)
{
    SPI_FLASH_SECTOR_ERASE_EVENT(flashAddress);
    SPI_FLASH_OP_START();
    hal_SpiFlashEraseWithOpcode(OPCODE_SECTOR_ERASE, flashAddress);
    SPI_FLASH_OP_END();
}

void hal_SpiFlashBlockErase(uint32_t flashAddress)
{
    SPI_FLASH_BLOCK_ERASE_EVENT(flashAddress);
    SPI_FLASH_OP_START();
    hal_SpiFlashEraseWithOpcode(OPCODE_BLOCK_ERASE, flashAddress);
    SPI_FLASH_OP_END();
}

void hal_SpiFlashBlock32KErase(uint32_t flashAddress)
{
    SPI_FLASH_BLOCK32K_ERASE_EVENT(flashAddress);
    SPI_FLASH_OP_START();
    hal_SpiFlashEraseWithOpcode(OPCODE_BLOCK32K_ERASE, flashAddress);
    SPI_FLASH_OP_END();
}

static void hal_SpiFlashProgram(uint32_t flashAddress, const void *data, unsigned size)
{
    SPI_FLASH_PROGRAM_EVENT(flashAddress);
    SPI_FLASH_PROGRAM_SIZE_EVENT(flashAddress, size);
    SPI_FLASH_OP_START();

    uint8_t ramdata[SPI_FLASH_PAGE_SIZE];
    memcpy(ramdata, data, size);

    uint32_t sc = hal_SpiFlashWaitOtherMaster();

    SPI_FLASH_OP_TASK();
    bool checkirq = gSpiFlashCtx.checkirq;
    if (sc == 0 || OPCODE_PROGRAM_SUSPEND == 0 || OPCODE_PROGRAM_RESUME == 0)
        checkirq = false;

    hal_SpiFlashEnableAhbRead(false);
    hal_SpiCmdWriteEnable();
    hal_SpiCmdProgram(flashAddress, ramdata, size);

    for (;;)
    {
        hal_SpiFlashCheckVoc();
        if (hal_SpiFlashCheckWipDone())
        {
            hal_SpiCmdHighPerfMode();
            hal_SpiCmdWriteDisable();
            hal_SpiFlashEnableAhbRead(true);
            hal_SysExitCriticalSection(sc);
            SPI_FLASH_OP_END();
            return;
        }

        if (checkirq)
        {
            if (hwp_sysIrq->Cause != 0)
            {
                hal_SpiCmdProgramSuspend();
                hal_SpiFlashWaitWipDone();
                hal_SpiCmdHighPerfMode();
                hal_SpiFlashEnableAhbRead(true);
                hal_SysExitCriticalSection(sc);

                NOP_FOR_EXIT_SC();
                sc = hal_SpiFlashWaitOtherMaster();
                hal_SpiFlashEnableAhbRead(false);
                hal_SpiCmdProgramResume();
            }
        }
    }
}

static void hal_SpiFlashInit(void)
{
    uint32_t id = hal_SpiCmdWithRx(OPCODE_READ_ID, false, NULL, 0, 3);
    mon_Event(id);

    gSpiFlashCtx.id = id;
    gSpiFlashCtx.checkirq = false;
    gSpiFlashCtx.lock.callback = NULL;
    gSpiFlashCtx.unlock.callback = NULL;
    gSpiFlashCtx.manufacturerID = (id & 0xff);
    gSpiFlashCtx.memoryTypeID = ((id >> 8) & 0xff);
    gSpiFlashCtx.capacityID = ((id >> 16) & 0xff);

    gSpiFlashCtx.opcodeHighPerfMode = 0;
    if (gSpiFlashCtx.manufacturerID == MEMD_FLASH_WINBOND)
        gSpiFlashCtx.opcodeHighPerfMode = 0xa3;
}

void hal_SpiFlashOpen(void)
{
#ifdef CHIP_DIE_8909
// TODO:
#else
#if defined(SPI_FLSH_52M)
    hwp_sysCtrl->Cfg_Clk_SpiFlash = SYS_CTRL_SPIFLASH_FREQ_104M;
#elif defined(SPI_FLSH_78M)
    hwp_sysCtrl->Cfg_Clk_SpiFlash = SYS_CTRL_SPIFLASH_FREQ_156M;
#elif defined(SPI_FLSH_89M)
    hwp_sysCtrl->Cfg_Clk_SpiFlash = SYS_CTRL_SPIFLASH_FREQ_178M;
#elif defined(SPI_FLSH_104M)
    hwp_sysCtrl->Cfg_Clk_SpiFlash = SYS_CTRL_SPIFLASH_FREQ_208M;
#endif
#endif

    uint32_t config = SPI_FLASH_SAMPLE_DELAY(1) |
                      SPI_FLASH_CLK_DIVIDER(2) |
                      SPI_FLASH_QUAD_MODE_QUAD_READ |
                      0 /* SPI_FLASH_SPI_WPROTECT_PIN */ |
                      0 /* SPI_FLASH_SPI_HOLD_PIN */ |
                      0 /* SPI_FLASH_CMD_QUAD */ |
                      SPI_FLASH_TX_RX_SIZE(0);

#if defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
    config |= SPI_FLASH_BYPASS_START_CMD;
#endif
    hwp_spiFlash->spi_config = config;

    hal_SpiCmdEnableReset();
    hal_SpiCmdReset();
    hal_SysWaitMicrosecond(DELAY_TRST);

    hal_SpiFlashInit();

    uint8_t sr2 = hal_SpiFlashReadStatus2();
    if ((sr2 & STREG2_QE) == 0)
    {
        sr2 |= STREG2_QE;
        uint8_t sr1 = hal_SpiFlashReadStatus();
        hal_SpiCmdWriteEnable();
        hal_SpiFlashWriteStatus(sr1, sr2);
        hal_SpiFlashWaitWipDone();
        hal_SpiCmdWriteDisable();
    }
}

uint32_t hal_SpiFlashGetID(void)
{
    return (gSpiFlashCtx.capacityID << 16) |
           (gSpiFlashCtx.memoryTypeID << 8) |
           (gSpiFlashCtx.manufacturerID);
}

void hal_SpiFlashSetLockCallback(HAL_CALLBACK_FUNCTION_T lock, void *param)
{
    gSpiFlashCtx.lock.callback = lock;
    gSpiFlashCtx.lock.param = param;
}

void hal_SpiFlashLock(void)
{
    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);
}

void hal_SpiFlashUnlock(void)
{
    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

void hal_SpiFlashSetUnlockCallback(HAL_CALLBACK_FUNCTION_T unlock, void *param)
{
    gSpiFlashCtx.unlock.callback = unlock;
    gSpiFlashCtx.unlock.param = param;
}

void hal_SpiFlashReset(void)
{
    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdEnableReset();
    hal_SpiCmdReset();
    hal_SysWaitMicrosecond(DELAY_TRST);
    hal_SysExitCriticalSection(sc);
}

void hal_SpiFlashSleep(void)
{
    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiFlashEnableAhbRead(false);
    hal_SpiCmdPowerDown();
    hal_SysExitCriticalSection(sc);
}

void hal_SpiFlashWakeup(void)
{
    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdReleasePowerDown();
    hal_SpiCmdHighPerfMode();
    hal_SysWaitMicrosecond(DELAY_TRES1);
    hal_SpiFlashEnableAhbRead(true);
    hal_SysExitCriticalSection(sc);
}

void *hal_SpiFlashMapUncacheAddress(uint32_t flashAddress)
{
    return (void *)KSEG1(flashAddress | 0x08000000);
}

uint32_t hal_SpiFlashOffset(void *address)
{
    return (uint32_t)address & 0x00ffffff;
}

void hal_SpiFlashSetCheckIrq(bool checkirq) { gSpiFlashCtx.checkirq = checkirq; }
unsigned hal_SpiFlashGetPageSize(void) { return SPI_FLASH_PAGE_SIZE; }
unsigned hal_SpiFlashGetSectorSize(void) { return SPI_FLASH_SECTOR_SIZE; }
unsigned hal_SpiFlashGetBlockSize(void) { return SPI_FLASH_BLOCK_SIZE; }
unsigned hal_SpiFlashGetSize(void) { return 1 << gSpiFlashCtx.capacityID; }

uint16_t hal_SpiFlashReadStatusRegister(void)
{
    uint32_t sc = hal_SysEnterCriticalSection();
    uint8_t val0 = hal_SpiFlashReadStatus();
    uint8_t val1 = hal_SpiFlashReadStatus2();
    uint16_t reg = (val1 << 8) | val0;
    hal_SysExitCriticalSection(sc);
    return reg;
}

void hal_SpiFlashWriteStatusRegister(uint16_t val, uint16_t mask)
{
    if (mask == 0)
        return;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    uint32_t reg = hal_SpiFlashReadStatusRegister();
    reg = (reg & ~mask) | (val & mask);
    hal_SpiCmdWriteEnable();
    hal_SpiFlashWriteStatus(reg & 0xff, (reg >> 8) & 0xff);
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

uint8_t hal_SpiFlashReadStatusRegisterEx(uint8_t n)
{
    return (n == 1) ? hal_SpiFlashReadStatus() : (n == 2) ? hal_SpiFlashReadStatus2() : (n == 3) ? hal_SpiFlashReadStatus3() : 0;
}

void hal_SpiFlashWriteStatusRegisterEx(uint8_t n, uint8_t val)
{
    if (n < 1 && n > 3)
        return;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdWriteEnable();
    hal_SpiFlashWriteStatusEx(n, val);
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

void hal_SpiFlashWriteVolatileStatusRegisterEx(uint8_t n, uint8_t val)
{
    if (n < 1 && n > 3)
        return;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdWriteVolatileEnable();
    hal_SpiFlashWriteStatusEx(n, val);
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

bool hal_SpiFlashWrite(uint32_t flashAddress, const void *data, unsigned size)
{
    uint32_t capacity = (1 << gSpiFlashCtx.capacityID);
    if (flashAddress >= capacity ||
        (flashAddress + size) > capacity ||
        data == NULL)
        return false;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

#ifdef DBG_VERIFY_PROGRAM
    unsigned total = size;
#endif
    while (size > 0)
    {
        unsigned nextPage = (flashAddress + SPI_FLASH_PAGE_SIZE) & ~SPI_FLASH_PAGE_MASK;
        unsigned trans = nextPage - flashAddress;
        if (trans > size)
            trans = size;

        hal_SpiFlashProgram(flashAddress, data, trans);
        flashAddress += trans;
        data += trans;
        size -= trans;
    }

#ifdef DBG_VERIFY_PROGRAM
    const uint8_t *fp = hal_SpiFlashMapUncacheAddress(flashAddress - total);
    const uint8_t *fd = (const uint8_t *)data - total;
    if (memcmp(fp, fd, total) != 0)
        asm("break 1");
#endif

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
    return true;
}

bool hal_SpiFlashErase(uint32_t flashAddress, unsigned size)
{
    uint32_t capacity = (1 << gSpiFlashCtx.capacityID);
    if (flashAddress >= capacity ||
        (flashAddress + size) > capacity ||
        (flashAddress & SPI_FLASH_SECTOR_MASK) != 0 ||
        (size & SPI_FLASH_SECTOR_MASK) != 0)
        return false;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

#ifdef DBG_VERIFY_ERASE
    unsigned total = size;
#endif
    while (size > 0)
    {
        if ((flashAddress & SPI_FLASH_BLOCK_MASK) == 0 && size >= SPI_FLASH_BLOCK_SIZE)
        {
            hal_SpiFlashBlockErase(flashAddress);
            flashAddress += SPI_FLASH_BLOCK_SIZE;
            size -= SPI_FLASH_BLOCK_SIZE;
        }
        else if ((flashAddress & SPI_FLASH_BLOCK32K_MASK) == 0 && size >= SPI_FLASH_BLOCK32K_SIZE)
        {
            hal_SpiFlashBlock32KErase(flashAddress);
            flashAddress += SPI_FLASH_BLOCK32K_SIZE;
            size -= SPI_FLASH_BLOCK32K_SIZE;
        }
        else
        {
            hal_SpiFlashSectorErase(flashAddress);
            flashAddress += SPI_FLASH_SECTOR_SIZE;
            size -= SPI_FLASH_SECTOR_SIZE;
        }
    }

#ifdef DBG_VERIFY_ERASE
    const uint8_t *fp = hal_SpiFlashMapUncacheAddress(flashAddress - total);
    for (unsigned n = 0; n < total; n++)
    {
        if (fp[n] != 0xff)
            asm("break 1");
    }
#endif

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
    return true;
}

void hal_SpiFlashChipErase(void)
{
    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdWriteEnable();
    hal_SpiCmdChipErase();
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

void hal_SpiFlashSecurityErase(uint32_t address)
{
    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdWriteEnable();
    hal_SpiCmdSecurityErase(address);
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

void hal_SpiFlashSecurityWrite(uint32_t address, const void *data, unsigned size)
{
    if (data == NULL)
        return;

    HAL_RUN_CALLBACK(gSpiFlashCtx.lock);

    uint32_t sc = hal_SysEnterCriticalSection();
    hal_SpiCmdWriteEnable();
    hal_SpiCmdSecurityProgram(address, (const uint8_t *)data, size);
    sc = hal_SpiFlashWaitWipDoneLocked(sc);
    hal_SpiCmdWriteDisable();
    hal_SysExitCriticalSection(sc);

    HAL_RUN_CALLBACK(gSpiFlashCtx.unlock);
}

void hal_SpiFlashSecurityRead(uint32_t address, void *data, unsigned size)
{
    if (data == NULL)
        return;

    uint8_t *p = (uint8_t *)data;
    while (size > 0)
    {
        unsigned trans = (size > 4) ? 4 : size;
        uint32_t sc = hal_SysEnterCriticalSection();
        uint32_t val = hal_SpiCmdSecurityRead(address, trans);
        hal_SysExitCriticalSection(sc);
        for (unsigned n = 0; n < trans; n++)
        {
            *p++ = val & 0xff;
            val >>= 8;
        }
        address += trans;
        size -= trans;
    }
}
