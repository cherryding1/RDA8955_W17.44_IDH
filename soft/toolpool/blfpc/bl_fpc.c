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

#include "bl_map.h"
#include "bl_crc.h"
#include "bl_malloc.h"
#include "bl_dma.h"
#include "bl_entry.h"
#include "bl_platform.h"
#include "bl_spi_flash.h"
#include "bl_log.h"
#include "bl_pmd.h"
#include "blfpcp_version.h"
#include "flash_prog_map.h"
#include "memd_map.h"

#define BOOT_FLASH_PROGRAMMED_PATTERN 0xD9EF0045

#ifdef FASTPF_USE_USB
// It is too hard to include "boot_usb_monitor.h"
#define BOOT_USB_CONTEXT_SIZE (5 * 1024) // BOOT_HOST_USB_MONITOR_CTX_T
extern void boot_HostUsbEvent(uint32_t event);
extern void boot_HostUsbRecv(void);
extern bool boot_HostUsbSend(void);
extern void boot_HostUsbOpen(void *context, void *, void *);
#endif

// FPC factory data will only follow the same block management.
// So, we can't include tgt_factory.h directory
#define FPC_FACTORY_BLOCK_SIZE 1024
#define FPC_FACTORY_DATA_SIZE 8192
#define FPC_FACTORY_BLOCK_COUNT (FPC_FACTORY_DATA_SIZE / FPC_FACTORY_BLOCK_SIZE)
#define FPC_FACTORY_VERSION 0xFAC00200

typedef struct
{
    uint32_t version;
    uint8_t data[(FPC_FACTORY_BLOCK_SIZE - 16)];
    uint32_t sequence;
    uint32_t crc;
    uint32_t crcInverted;
} FPC_FACTORY_BLOCK_T;

static const BL_MAP_VERSION_T gFpcVersion = BLFPC_VERSION_STRUCT;
static volatile FLASH_PROG_MAP_ACCESS_T gFpcAccess SECTION_SRAM_UCBSS;

static const BL_MAP_VERSION_T gMemdVersion = {
    .revision = BLFPC_VERSION_REVISION, // It is the same
    .number = 2,                        // version 2
    .date = BLFPC_VERSION_DATE,
    .str = "MEMD for " STRINGIFY_VALUE(MEMD_FLSH_MODEL) " Branch: unknown",
};
static const MEMD_MAP_ACCESS_T gMemdAccess = {
    .ramPhySize = 0, // TODO
    .calibBase = CALIB_BASE,
    .calibSize = CALIB_SIZE,
    .calibRfBase = CALIB_BASE + 0x2000,
    .factBase = FACT_SETTINGS_BASE,
    .factSize = FACT_SETTINGS_SIZE,
    .flashLayout = NULL,
    .flashSize = FLASH_SIZE,
    .userBase = 0, // it will change for targets
    .userSize = 0, // it will change for targets
};

static void fpc_send_event(uint32_t event)
{
#ifdef FASTPF_USE_HOST
    bl_send_event(event);
#endif
#ifdef FASTPF_USE_USB
    boot_HostUsbEvent(event);
#endif
}

static bool fpc_word_cmp(uint32_t *data, uint32_t c, int words)
{
    for (int n = 0; n < words; n++)
    {
        if (*data++ != c)
            return false;
    }
    return true;
}

static void fpc_sector_limites(uint32_t flashAddr, uint32_t *start, uint32_t *end)
{
    // FPC_ERASE_SECTOR should be obsolete.
    if ((flashAddr & (SPI_FLASH_BLOCK_SIZE - 1)) == 0)
    {
        *start = flashAddr & ~(SPI_FLASH_BLOCK_SIZE - 1);
        *end = *start + SPI_FLASH_BLOCK_SIZE;
    }
    else
    {
        *start = flashAddr & ~(SPI_FLASH_SECTOR_SIZE - 1);
        *end = *start + SPI_FLASH_SECTOR_SIZE;
    }
}

static bool fpc_erase(uint32_t flashAddr, unsigned size)
{
    return bl_SpiFlashErase(flashAddr, size);
}

static bool fpc_sector_erase(uint32_t flashAddr)
{
    uint32_t start, end;
    fpc_sector_limites(flashAddr, &start, &end);
    return bl_SpiFlashErase(start, end - start);
}

static bool fpc_check_fcs(uint32_t size, uint32_t *params)
{
    uint32_t firstbad = -1U;
    uint32_t *badptr = params;
    for (unsigned n = 0; n < size; n++)
    {
        uint32_t flashAddr = params[0];
        uint32_t flashSize = params[1];
        uint32_t fcs = params[2];

        uint32_t calcFcs = bl_dma_calc_fcs(SPI_FLASH_UNCACHE_ADDRESS(flashAddr), flashSize);
        if (calcFcs != fcs)
        {
            if (firstbad == -1U)
                firstbad = n;
            params[2] = calcFcs;
        }
        params += 3;
    }
    if (firstbad != -1U)
    {
        *badptr = firstbad;
        return false;
    }
    return true;
}

static bool fpc_check_calib_crc(uint32_t flashAddr)
{
    uint8_t *calib = (uint8_t *)SPI_FLASH_UNCACHE_ADDRESS(flashAddr);
    if (calib[3] == 0xca && calib[2] == 0x1b && ((calib[1] << 8) | calib[0]) > 0x0203)
    {
        uint32_t fcs = bl_read_fcs(&calib[0x1cf8]);        // IT IS MAGIC
        uint32_t calcFcs = bl_dma_calc_fcs(calib, 0x1360); // IT IS MAGIC
        return (fcs == calcFcs);
    }
    return false;
}

static bool fpc_erase_program(uint32_t flashAddr, uint32_t size, uint8_t *ramAddr, uint32_t mask)
{
    if (!bl_SpiFlashErase(flashAddr, size))
        return false;

    if ((mask & 0xFFFF) == 0xFFFF)
    {
        if (!bl_SpiFlashWrite(flashAddr, ramAddr, size))
            return false;
    }
    else
    {
        uint32_t trans = (size / 16);
        for (int n = 0; n < 16; n++)
        {
            if ((mask & (1 << n)) != 0)
            {
                if (!bl_SpiFlashWrite(flashAddr, ramAddr, trans))
                    return false;
            }

            flashAddr += trans;
            ramAddr += trans;
        }
    }
    return true;
}

static bool fpc_overwrite(uint32_t flashAddr, uint32_t size, uint8_t *ramAddr)
{
    uint32_t offset = flashAddr & 0xFFF;
    uint8_t *back = bl_ddr_malloc(0x1000);

    while (size > 0)
    {
        uint32_t aligned = flashAddr - offset;
        uint32_t trans = (offset + size > 0x1000) ? (0x1000 - offset) : size;
        if (offset == 0 && trans == 0x1000)
        {
            memcpy(back, ramAddr, 0x1000);
        }
        else
        {
            void *flashPtr = SPI_FLASH_UNCACHE_ADDRESS(aligned);
            memcpy(back, flashPtr, 0x1000);
            memcpy(back + offset, ramAddr, trans);
        }

        if (!bl_SpiFlashErase(aligned, 0x1000) ||
            !bl_SpiFlashWrite(aligned, back, 0x1000))
            goto failed;

        size -= trans;
        flashAddr += trans;
        ramAddr += trans;
        offset = 0; // only not-aligned at the 1st one
    }

    bl_free(back);
    return true;

failed:
    bl_free(back);
    return false;
}

bool fpc_factory_block_is_valid(FPC_FACTORY_BLOCK_T *block)
{
    if ((block->version & 0xFFFFFF00) != FPC_FACTORY_VERSION)
        return FALSE;

    if ((block->sequence & 1) != 0)
        return FALSE;

    if (block->crc != ~block->crcInverted)
        return FALSE;

    if (bl_calc_crc(&block->version, FPC_FACTORY_BLOCK_SIZE - 8) != block->crc)
        return FALSE;

    return TRUE;
}

int fpc_find_factory_block(FPC_FACTORY_BLOCK_T *blocks)
{
    uint32_t sequence = 0;
    int index = -1;
    int n;

    for (n = 0; n < FPC_FACTORY_BLOCK_COUNT; n++)
    {
        if (fpc_factory_block_is_valid(&blocks[n]) &&
            blocks[n].sequence >= sequence)
        {
            index = n;
            sequence = blocks[n].sequence;
        }
    }

    return index;
}

bool fpc_factory_read(uint32_t flashAddr, uint8_t *ramAddr)
{
    FPC_FACTORY_BLOCK_T *blocks = (FPC_FACTORY_BLOCK_T *)SPI_FLASH_UNCACHE_ADDRESS(flashAddr);
    int index = fpc_find_factory_block(blocks);
    if (index < 0)
        memset(ramAddr, 0xFF, FPC_FACTORY_BLOCK_SIZE);
    else
        memcpy(ramAddr, &blocks[index], FPC_FACTORY_BLOCK_SIZE);
    return true;
}

bool fpc_factory_write(uint32_t flashAddr, UINT8 *ramAddr)
{
    VOID *factAddr = (VOID *)SPI_FLASH_UNCACHE_ADDRESS((uint32_t)flashAddr);
    FPC_FACTORY_BLOCK_T *blocks = (FPC_FACTORY_BLOCK_T *)factAddr;
    FPC_FACTORY_BLOCK_T *block = (FPC_FACTORY_BLOCK_T *)ramAddr;
    int n;
    int index;
    int nextIndex;

    index = fpc_find_factory_block(blocks);
    if (index < 0)
    {
        block->sequence = 0;
        nextIndex = 0;

        // erase both sector if no valid block
        if (!fpc_erase(flashAddr, FPC_FACTORY_DATA_SIZE))
            return false;
    }
    else
    {
        block->sequence = blocks[index].sequence + 2;

        // search empty block in current sector
        for (n = 1; n <= FPC_FACTORY_BLOCK_COUNT / 2; n++)
        {
            nextIndex = (index + n) % (FPC_FACTORY_BLOCK_COUNT);
            if (nextIndex == 0 || nextIndex == FPC_FACTORY_BLOCK_COUNT / 2)
            {
                if (!fpc_erase(flashAddr + nextIndex * FPC_FACTORY_BLOCK_SIZE,
                               FPC_FACTORY_DATA_SIZE / 2))
                    return false;
                break; // start of next sector
            }

            if (fpc_word_cmp((uint32_t *)&blocks[nextIndex], 0xFFFFFFFF,
                             FPC_FACTORY_BLOCK_SIZE / 4))
                break; // first empty block
        }
    }

    block->crc = bl_calc_crc(&block->version, FPC_FACTORY_BLOCK_SIZE - 8);
    block->crcInverted = ~block->crc;
    if (!bl_SpiFlashWrite(flashAddr + nextIndex * FPC_FACTORY_BLOCK_SIZE,
                          block, FPC_FACTORY_BLOCK_SIZE))
        return false;
    return true;
}

static int fpc_cmd_done(bool ok, volatile FPC_COMMAND_DATA_T *cmds, int i)
{
    if (ok)
    {
        cmds[i].cmd = FPC_DONE;
        fpc_send_event(EVENT_FLASH_PROG_READY + i);
    }
    else
    {
        cmds[i].cmd = FPC_ERROR;
        fpc_send_event(EVENT_FLASH_PROG_ERROR + i);
    }
    return i ^ 1;
}

static void fpc_shutdown(void)
{
    bl_delay_us(20 * 1000);
    bl_platform_shutdown();

    for (;;)
    {
#ifdef FASTPF_USE_USB
        boot_HostUsbRecv();
        boot_HostUsbSend();
#endif
    }
}

#ifdef GOKE_FLASH_TOOL_SUPPORT
#include "goke_flash_tool.h"

static bool fpc_gkft_check_fcs(uint32_t size, uint32_t *params)
{
    uint8_t *data = (uint8_t *)gkft_image_data();
    bl_dcache_wbinv_all();
    for (unsigned n = 0; n < size; n++)
    {
        uint32_t dataOffset = params[0];
        uint32_t dataSize = params[1];
        uint32_t fcs = params[2];

        uint32_t calcFcs = bl_dma_calc_fcs(UNCACHE_ADDRESS(data + dataOffset), dataSize);
        if (calcFcs != fcs)
            return false;

        params += 3;
    }
    return true;
}

static void fpc_gkft_progress(uint32_t size, uint32_t total, unsigned long param)
{
    *((uint32_t *)param) = (size * 100)/total;
}
#endif // GOKE_FLASH_TOOL_SUPPORT

void bl_fpc(uint32_t param)
{
    bl_init();

#ifdef FASTPF_USE_USB
    boot_HostUsbInit();
    boot_HostUsbRecv();
    boot_HostUsbSend();
#endif

    unsigned memsize = 3 * FPC_BUFFER_SIZE;
    uint32_t pool = (uint32_t)bl_ddr_malloc(memsize + 16);
    uint32_t ucpool = (uint32_t)UNCACHE_ADDRESS((pool + 15) & ~15);

    memset((uint32_t *)&gFpcAccess, 0, sizeof(gFpcAccess));
    gFpcAccess.protocolVersion.major = 0xfa01;
    gFpcAccess.protocolVersion.minor = 0xfb05;
    gFpcAccess.dataBufferA = (uint8_t *)ucpool;
    gFpcAccess.dataBufferB = (uint8_t *)(ucpool + FPC_BUFFER_SIZE);
    gFpcAccess.dataBufferC = (uint8_t *)(ucpool + 2 * FPC_BUFFER_SIZE);
    gFpcAccess.fpcSize = FPC_BUFFER_SIZE;

    bl_watchdog_stop();
    bl_map_init();
    bl_map_register(BL_MAP_FLASH_PROG, (void *)&gFpcVersion, (void *)&gFpcAccess);
    bl_map_register(BL_MAP_MEMD, (void *)&gMemdVersion, (void *)&gMemdAccess);

    fpc_send_event(EVENT_FLASH_PROG_READY);

    volatile FPC_COMMAND_DATA_T *cmds = &gFpcAccess.commandDescr[0];
    uint32_t magic = BOOT_FLASH_PROGRAMMED_PATTERN;
    uint32_t bootFlashAddr = 0;
    bool bootWritten = false;
    int i = 0;

    for (;;)
    {
#ifdef FASTPF_USE_USB
        boot_HostUsbRecv();
        boot_HostUsbSend();
#endif

        uint32_t cmd = cmds[i].cmd;
        uint32_t *ramwords = (uint32_t *)cmds[i].ramAddr;
        bool cmdok = true;

        switch (cmd)
        {
        case FPC_NONE:
        case FPC_DONE:
        case FPC_ERROR:
        case FPC_FCS_ERROR:
            break;

        case FPC_SHUTDOWN:
            i = fpc_cmd_done(true, cmds, i);
            fpc_shutdown();
            break;

        case FPC_PING:
            i = fpc_cmd_done(true, cmds, i);
            break;

        case FPC_PROGRAM:
            if ((uint32_t)cmds[i].flashAddr == bootFlashAddr)
                bootWritten = true;

            cmdok = bl_SpiFlashWrite((uint32_t)cmds[i].flashAddr, cmds[i].ramAddr, cmds[i].size);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_ERASE_SECTOR:
            cmdok = fpc_sector_erase((uint32_t)cmds[i].flashAddr);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_ERASE_CHIP:
            bl_SpiFlashChipErase();
            i = fpc_cmd_done(true, cmds, i);
            break;

        case FPC_GET_FINALIZE_INFO:
            cmds[i].size = 0;
            if (bootWritten)
            {
                ramwords[0] = bootFlashAddr;
                ramwords[1] = magic;
                cmds[i].size = 1;
            }
            i = fpc_cmd_done(true, cmds, i);
            break;

        case FPC_CHECK_FCS:
            cmdok = fpc_check_fcs(cmds[i].size, ramwords);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_CHECK_CALIBRFPART_CRC:
            cmdok = fpc_check_calib_crc((uint32_t)cmds[i].flashAddr);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_END:
            if (bootWritten)
            {
                bl_SpiFlashWrite(bootFlashAddr, &magic, sizeof(magic));
                if (*(uint32_t *)SPI_FLASH_UNCACHE_ADDRESS(bootFlashAddr) != magic)
                    cmdok = false;
            }
            bl_pmd_set_lcd_level(1);
            bl_pmd_set_keypad_level(1);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_SET_BOOT_SECTOR:
            bootFlashAddr = (uint32_t)cmds[i].flashAddr;
            bootWritten = false;
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_UNSET_BOOT_SECTOR:
            bootFlashAddr = -1U;
            bootWritten = false;
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_ERASE_PROGRAM:
            if ((uint32_t)cmds[i].flashAddr == bootFlashAddr)
                bootWritten = true;

            cmdok = fpc_erase_program((uint32_t)cmds[i].flashAddr, cmds[i].size, cmds[i].ramAddr, cmds[i].fcs);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_ERASE:
            cmdok = fpc_erase((uint32_t)cmds[i].flashAddr, cmds[i].size);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_OVERWRITE:
            cmdok = fpc_overwrite((uint32_t)cmds[i].flashAddr, cmds[i].size, cmds[i].ramAddr);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_LIGHT_ON:
            bl_pmd_set_lcd_level(1);
            bl_pmd_set_keypad_level(1);
            bl_pmd_enable_camera_flash(true);
            i = fpc_cmd_done(true, cmds, i);
            break;

        case FPC_FACTORY_READ:
            cmdok = fpc_factory_read((uint32_t)cmds[i].flashAddr, cmds[i].ramAddr);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_FACTORY_WRITE:
            cmdok = fpc_factory_write((uint32_t)cmds[i].flashAddr, cmds[i].ramAddr);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_RESTART:
            i = fpc_cmd_done(true, cmds, i);
            bl_delay_us(100 * 1000);
            bl_platform_reset();
            break;

#ifdef GOKE_FLASH_TOOL_SUPPORT
        case FPC_GKFT_INITIAL:
            {
                uint32_t uartid = ramwords[0];
                uint32_t image_size = ramwords[1];
                bool need_32k = ramwords[2] != 0;
                i = fpc_cmd_done(gkft_init(uartid, image_size, need_32k), cmds, i);
            }
            break;

        case FPC_GKFT_UPDATE_IMAGE:
            i = fpc_cmd_done(gkft_fill_data(cmds[i].ramAddr, cmds[i].size), cmds, i);
            break;

        case FPC_GKFT_VERIFY_IMAGE:
            cmdok = fpc_gkft_check_fcs(cmds[i].size, ramwords);
            i = fpc_cmd_done(cmdok, cmds, i);
            break;

        case FPC_GKFT_BURN_IMAGE:
            *ramwords = 0;
            cmdok = gkft_burn_image(fpc_gkft_progress, (unsigned long)ramwords);
            gkft_destroy();
            i = fpc_cmd_done(cmdok, cmds, i);
            break;
#endif // GOKE_FLASH_TOOL_SUPPORT

        default:
            i = fpc_cmd_done(false, cmds, i);
            break;
        }
    }
}

void bl_main(uint32_t param)
{
    // IT IS MAGIC: boot monitor will use more than 4KB stack for USB context.
    // And we will reuse the memory in stack.
    extern char __sram_top[];
    bl_run_with_stack(param, bl_fpc, __sram_top - 0x1400);
}
