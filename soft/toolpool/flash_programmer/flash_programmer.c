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

#include "flash_prog_map.h"

#include "tgt_memd_cfg.h"
#include "memd_m.h"
#include "pmd_m.h"
#include "boot_usb_monitor.h"

#include "hal_sys.h"
#include "hal_debug.h"
#include "hal_dma.h"
#include "hal_clk.h"
#include "hal_timers.h"
#include "hal_ebc.h"
#include "hal_map_engine.h"
#include "hal_spi_flash.h"

// define the folowing to see the error code returned by the flash driver as host events
#define DEBUG_EVENTS
#ifdef DEBUG_EVENTS
#include "hal_host.h"
#endif

extern VOID mon_Event(UINT32 ch);
extern VOID boot_HostUsbEvent(UINT32 ch);
extern VOID *memcpy(VOID *dest, CONST VOID *src, UINT32 n);
extern void *memset(VOID *dest, int c, UINT32 n);
extern UINT32 vds_CRC32(CONST VOID *pvData, INT32 iLen);

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
#define SEND_EVENT boot_HostUsbEvent
#else
#define SEND_EVENT mon_Event
#endif

#ifndef FLASH_USB_SEND_EVENT_DELAY
#define FLASH_USB_SEND_EVENT_DELAY 8192 // 0.5s
#endif                                  /* FLASH_USB_SEND_EVENT_DELAY */

#define FP_DATA_BUFFER_LOCATION __attribute__((section(".ramucbss")))

#ifdef CHIP_DIE_8955
#define HAL_UNCACHED_DATA_INTERNAL __attribute__((section(".sramucdata")))

HAL_UNCACHED_DATA_INTERNAL BOOT_HOST_USB_MONITOR_CTX_T g_flashProgUsbContext;
#else
BOOT_HOST_USB_MONITOR_CTX_T g_flashProgUsbContext;
#endif
//-----------------------------
// Three Data Buffers are available for CoolWatcher
//-----------------------------
#if (FPC_BUFFER_SIZE / 4 * 4 != FPC_BUFFER_SIZE)
#error "FPC_BUFFER_SIZE should be aligned with word boundary"
#endif

PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferA[FPC_BUFFER_SIZE / 4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferB[FPC_BUFFER_SIZE / 4];
PROTECTED UINT32 FP_DATA_BUFFER_LOCATION g_dataBufferC[FPC_BUFFER_SIZE / 4];

EXPORT PUBLIC UINT32 _boot_loader_magic_number_in_flash;

PROTECTED HAL_MAP_VERSION_T HAL_MAP_VER_LOC g_flashProgVersion = {0, 0x20080121, 0, "Flash Programmer Version 1"};
PUBLIC FLASH_PROG_MAP_ACCESS_T HAL_MAP_ACCESS_LOC g_flashProgDescr = {
    {FPC_PROTOCOL_MAJOR, FPC_PROTOCOL_MINOR},
    {{FPC_NONE, (UINT32)0, 0, 0}, {FPC_NONE, (UINT32)0, 0, 0}},
    (UINT8 *)g_dataBufferA,
    (UINT8 *)g_dataBufferB,
    (UINT8 *)g_dataBufferC,
    FPC_BUFFER_SIZE};

#if !(CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
#error "error: You can t use FASTPF_USE_USB without CHIP_HAS_USB"
#endif

// =============================================================================
// FLASH_PROGRAMMER_SPECIAL_SECTOR_T
// -----------------------------------------------------------------------------
/// Describe the special sector (use a magic number the first word)
// =============================================================================
typedef struct
{
    UINT8 *address;
    UINT32 magicNumber;
} FLASH_PROGRAMMER_SPECIAL_SECTOR_T;

// Check whether flash programmer data buffer is accessible
PROTECTED BOOL fp_CheckDataBuffer(VOID)
{
    int i, j;
    static CONST UINT32 pattern[] = {0xa5a5a5a5, 0x5a5a5a5a, 0x4f35b7da, 0x8e354c91};
    static VOLATILE UINT32 *CONST dataAry[] = {
        (VOLATILE UINT32 *)&g_dataBufferA[0],
        (VOLATILE UINT32 *)&g_dataBufferA[FPC_BUFFER_SIZE / 4 - 1],
        (VOLATILE UINT32 *)&g_dataBufferB[0],
        (VOLATILE UINT32 *)&g_dataBufferB[FPC_BUFFER_SIZE / 4 - 1],
        (VOLATILE UINT32 *)&g_dataBufferC[0],
        (VOLATILE UINT32 *)&g_dataBufferC[FPC_BUFFER_SIZE / 4 - 1],
    };

    for (i = 0; i < sizeof(dataAry) / sizeof(dataAry[0]); i++)
    {
        for (j = 0; j < sizeof(pattern) / sizeof(pattern[0]); j++)
        {
            *(dataAry[i]) = pattern[j];
            if (*(dataAry[i]) != pattern[j])
            {
                SEND_EVENT(0xebc0eee2);
                SEND_EVENT(*(dataAry[i]));
                SEND_EVENT(pattern[j]);
                SEND_EVENT(i);
                SEND_EVENT(j);
                return FALSE;
            }
        }
    }

#ifndef NO_PSRAM_HIGH_ADDR_CHECK
    typedef struct
    {
        UINT32 startAddr;
        UINT32 len;
    } PSRAM_RANGE_T;

    UINT32 psramBase = (UINT32)hal_EbcGetCsAddress(HAL_EBC_SRAM);
    PSRAM_RANGE_T range[] = {
        {
            psramBase + 0x3c0000, 0x100,
        },
        {
            psramBase + 0x3fff00, 0x100,
        },
    };

    UINT32 psramAddr;
    for (i = 0; i < sizeof(range) / sizeof(range[0]); i++)
    {

        for (psramAddr = range[i].startAddr;
             psramAddr + 4 <= range[i].startAddr + range[i].len;
             psramAddr += 4)
        {
            for (j = 0; j < sizeof(pattern) / sizeof(pattern[0]); j++)
            {
                *(VOLATILE UINT32 *)psramAddr = pattern[j];
                if (*(VOLATILE UINT32 *)psramAddr != pattern[j])
                {
                    SEND_EVENT(0xebc0eee1);
                    SEND_EVENT(psramAddr);
                    SEND_EVENT(*(VOLATILE UINT32 *)psramAddr);
                    SEND_EVENT(pattern[j]);
                    SEND_EVENT(i);
                    SEND_EVENT(j);
                    return FALSE;
                }
            }
        }
    }
#endif

    return TRUE;
}

PRIVATE UINT32 FpcSpecialSector(UINT8 *flashAddr, FLASH_PROGRAMMER_SPECIAL_SECTOR_T ss[])
{
    UINT32 flags = 0;
    int cpt;
    for (cpt = 0; ss[cpt].magicNumber != 0x00000000; ++cpt)
    {
        if (flashAddr == ss[cpt].address)
            flags |= (1 << cpt);
    }
    return flags;
}

PRIVATE bool FpcProgram(UINT8 *flashAddr, UINT32 size, UINT8 *ramAddr)
{
    return hal_SpiFlashWrite((uint32_t)flashAddr, ramAddr, size);
}

PRIVATE bool FpcErase(UINT8 *flashAddr, UINT32 size)
{
    return hal_SpiFlashErase((uint32_t)flashAddr, size);
}

PRIVATE bool FpcEraseProgram(UINT8 *flashAddr, UINT32 size, UINT8 *ramAddr, UINT32 mask)
{
    if (!hal_SpiFlashErase((uint32_t)flashAddr, size))
        return false;

    if ((mask & 0xFFFF) == 0xFFFF)
    {
        if (!FpcProgram(flashAddr, size, ramAddr))
            return false;
    }
    else
    {
        UINT32 trans = (size / 16);
        for (int n = 0; n < 16; n++)
        {
            if ((mask & (1 << n)) != 0)
            {
                if (!FpcProgram(flashAddr, trans, ramAddr))
                    return false;
            }

            flashAddr += trans;
            ramAddr += trans;
        }
    }
    return true;
}

PRIVATE bool FpcReplace(UINT8 *flashAddr, UINT32 size, UINT8 *ramAddr)
{
    UINT32 offset = (UINT32)flashAddr & 0xFFF;
    UINT8 *back = ramAddr + (FPC_BUFFER_SIZE - 0x1000);

    if (offset + size > FPC_BUFFER_SIZE)
        return false;

    while (size > 0)
    {
        UINT8 *aligned = flashAddr - offset;
        UINT32 trans = (offset + size > 0x1000) ? (0x1000 - offset) : size;
        UINT32 bytes;
        VOID *alignedGlobal = (VOID *)hal_SpiFlashMapUncacheAddress((uint32_t)aligned);

        memcpy(back, alignedGlobal, 0x1000);
        memcpy(back + offset, ramAddr, trans);

        if (!hal_SpiFlashErase((uint32_t)aligned, 0x1000))
            return false;

        if (!hal_SpiFlashWrite((uint32_t)aligned, &bytes, 0x1000))
            return false;

        size -= trans;
        flashAddr += trans;
        ramAddr += trans;
        offset = 0; // only not-aligned at the 1st one
    }

    return true;
}

// FPC factory data will only follow the same block management.
// So, we can't include tgt_factory.h directory
#define FPC_FACTORY_BLOCK_SIZE 1024
#define FPC_FACTORY_DATA_SIZE 8192
#define FPC_FACTORY_BLOCK_COUNT (FPC_FACTORY_DATA_SIZE / FPC_FACTORY_BLOCK_SIZE)
#define FPC_FACTORY_VERSION 0xFAC00200

typedef struct
{
    UINT32 version;
    UINT8 data[(FPC_FACTORY_BLOCK_SIZE - 16)];
    UINT32 sequence;
    UINT32 crc;
    UINT32 crcInverted;
} FPC_FACTORY_BLOCK_T;

BOOL FpcFactoryBlockValid(FPC_FACTORY_BLOCK_T *block)
{
    if ((block->version & 0xFFFFFF00) != FPC_FACTORY_VERSION)
        return FALSE;

    if ((block->sequence & 1) != 0)
        return FALSE;

    if (block->crc != ~block->crcInverted)
        return FALSE;

    if (vds_CRC32(&block->version, FPC_FACTORY_BLOCK_SIZE - 8) != block->crc)
        return FALSE;

    return TRUE;
}

BOOL FpcWordCmp(UINT32 *data, UINT32 pattern, int words)
{
    int n;
    for (n = 0; n < words; n++)
    {
        if (data[n] != pattern)
            return FALSE;
    }
    return TRUE;
}

int FpcFindFactoryBlock(FPC_FACTORY_BLOCK_T *blocks)
{
    UINT32 sequence = 0;
    int index = -1;
    int n;

    for (n = 0; n < FPC_FACTORY_BLOCK_COUNT; n++)
    {
        if (FpcFactoryBlockValid(&blocks[n]) &&
            blocks[n].sequence >= sequence)
        {
            index = n;
            sequence = blocks[n].sequence;
        }
    }

    return index;
}

VOID FpcFactoryRead(UINT8 *flashAddr, UINT8 *ramAddr)
{
    VOID *factAddr = (VOID *)hal_SpiFlashMapUncacheAddress((uint32_t)flashAddr);
    FPC_FACTORY_BLOCK_T *blocks = (FPC_FACTORY_BLOCK_T *)factAddr;
    int index;

    index = FpcFindFactoryBlock(blocks);
    if (index < 0)
    {
        memset(ramAddr, 0xFF, FPC_FACTORY_BLOCK_SIZE);
    }
    else
    {
        memcpy(ramAddr, &blocks[index], FPC_FACTORY_BLOCK_SIZE);
    }
}

bool FpcFactoryWrite(UINT8 *flashAddr, UINT8 *ramAddr)
{
    VOID *factAddr = (VOID *)hal_SpiFlashMapUncacheAddress((uint32_t)flashAddr);
    FPC_FACTORY_BLOCK_T *blocks = (FPC_FACTORY_BLOCK_T *)factAddr;
    FPC_FACTORY_BLOCK_T *block = (FPC_FACTORY_BLOCK_T *)ramAddr;
    int n;
    int index;
    int nextIndex;

    index = FpcFindFactoryBlock(blocks);
    if (index < 0)
    {
        block->sequence = 0;
        nextIndex = 0;

        // erase both sector if no valid block
        if (!FpcErase(flashAddr, FPC_FACTORY_DATA_SIZE))
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
                if (!FpcErase(flashAddr + nextIndex * FPC_FACTORY_BLOCK_SIZE,
                              FPC_FACTORY_DATA_SIZE / 2))
                    return false;
                break; // start of next sector
            }

            if (FpcWordCmp((UINT32 *)&blocks[nextIndex], 0xFFFFFFFF, FPC_FACTORY_BLOCK_SIZE / 4))
                break; // first empty block
        }
    }

    block->crc = vds_CRC32(&block->version, FPC_FACTORY_BLOCK_SIZE - 8);
    block->crcInverted = ~block->crc;
    if (!hal_SpiFlashWrite((uint32_t)flashAddr + nextIndex * FPC_FACTORY_BLOCK_SIZE,
                           block, FPC_FACTORY_BLOCK_SIZE))
        return MEMD_ERR_WRITE;
    return MEMD_ERR_NO;
}

// This xcpu_main replaces the regular xcpu_main present in the platform library
PROTECTED int main(VOID)
{
    VOLATILE FPC_COMMAND_DATA_T *cmds =
        (FPC_COMMAND_DATA_T *)HAL_SYS_GET_UNCACHED_ADDR((UINT32)g_flashProgDescr.commandDescr);
    INT32 i = 0;
    UINT32 cpt;
    UINT32 cpt2;
    INT32 errorDuringCommand;
    INT32 status = MEMD_ERR_NO;
    FPC_COMMAND_TYPE_T c;
    UINT32 s;
    UINT32 sectorWrite = 0;
    HAL_DMA_FCS_CFG_T fcsCtx;
    UINT8 fcs[3];
    UINT32 *verifySectorSize;
    UINT32 *verifySectorAddress;
    UINT32 *verifyAwaitedFcs;
    UINT32 verifyFirstBadSector;
    UINT32 m_CalibRfCrcAdd = 0;

    FLASH_PROGRAMMER_SPECIAL_SECTOR_T flashSpecialSector[] = {
        {.address = 0x00000000, .magicNumber = 0xD9EF0045},
        {.address = 0x00000000, .magicNumber = 0x00000000},
    };

    // We come here without calling "hal_Open"
    // Minimal and necessary "Open" are needed

    hal_MapEngineOpen();
    pmd_Open(tgt_GetPmdConfig());
    pmd_SetPowerMode(PMD_IDLEPOWER);

    hal_SwRequestClk(FOURCC_TOOL, HAL_CLK_RATE_104M);
    hal_TimWatchDogClose();
    cmds[0].cmd = FPC_NONE;
    cmds[1].cmd = FPC_NONE;

    hal_SpiFlashOpen();

    if (!fp_CheckDataBuffer())
    {
        SEND_EVENT(EVENT_FLASH_PROG_MEM_RESET);
        hal_SysResetOut(TRUE);
        pmd_EnableMemoryPower(FALSE);
        hal_TimDelay(2 * HAL_TICK1S);
        pmd_EnableMemoryPower(TRUE);
        hal_TimDelay(10);
        hal_SysResetOut(FALSE);
        hal_TimDelay(10);

        hal_SpiFlashOpen();
        memd_RamOpen(tgt_GetMemdRamConfig());

        if (!fp_CheckDataBuffer())
        {
            SEND_EVENT(EVENT_FLASH_PROG_MEM_ERROR);
            while (1)
                ;
        }
    }

    // Record the communication structure into HAL Mapping Engine
    hal_MapEngineRegisterModule(HAL_MAP_ID_FLASH_PROG, &g_flashProgVersion, &g_flashProgDescr);
    memd_RegisterYourself();

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
    g_flashProgUsbContext.MonitorMode = BOOT_HOST_USB_MODE_BOOT;
    boot_HostUsbOpen(&g_flashProgUsbContext, 0, 0);
#endif

#ifdef CHIP_DIE_8955
    hal_TimDelay(16384 / 2);
    SEND_EVENT(EVENT_FLASH_PROG_READY);
#else
    SEND_EVENT(EVENT_FLASH_PROG_READY);
    /// send Encrypt Uid to PC
    SEND_EVENT(pmd_GetEncryptUid());
#endif
    while (1)
    {
#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
        boot_HostUsbRecv();
        boot_HostUsbSend();
#endif

        c = cmds[i].cmd;

        switch (c)
        {
        case FPC_NONE:
        case FPC_DONE:
        case FPC_ERROR:
        case FPC_FCS_ERROR:
        case FPC_FLASH_NOT_AT_FF:
            break;

        case FPC_SHUTDOWN:
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            while (TRUE)
            {
                PMD_CHARGER_STATUS_T sts = pmd_GetChargerStatus();
                if (sts == PMD_CHARGER_UNKNOWN || sts == PMD_CHARGER_UNPLUGGED)
                {
                    hal_SysShutdown();
                }
                hal_TimDelay(5);
            }
            break;

        case FPC_PING:
            cmds[i].cmd = FPC_DONE;
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            i ^= 1;
            break;

        case FPC_PROGRAM:
            errorDuringCommand = 0;

            // Do the RAM check if we are asked too.
            // Do also a check of the FLASH (verify if everything is at FF).
            // These operations are port of the intensive verify.
            if (cmds[i].fcs & 0x80000000)
            {
                fcsCtx.srcAddr = cmds[i].ramAddr;
                fcsCtx.transferSize = cmds[i].size;
                fcsCtx.userHandler = NULL;

                hal_DmaFcsStart(&fcsCtx);
                hal_DmaReadFcs(fcs);

                if ((cmds[i].fcs & 0x00FFFFFF) != (fcs[0] << 16 | (fcs[1] << 8) | fcs[2]))
                {
                    errorDuringCommand = 1;
                    cmds[i].cmd = FPC_FCS_ERROR; // FCS error
                }
                else
                {
                    // Verify the content of the flash, if every byte is at FF
                    verifySectorAddress = (UINT32 *)hal_SpiFlashMapUncacheAddress((uint32_t)cmds[i].flashAddr);
                    for (cpt = 0; cpt < cmds[i].size; cpt += 4, verifySectorAddress++)
                    {
                        if (*verifySectorAddress != 0xFFFFFFFF)
                        {
                            errorDuringCommand = 1;
                            cmds[i].cmd = FPC_FLASH_NOT_AT_FF; // FCS error
                        }
                    }
                }
            }

            //Everything's ok til now
            if (errorDuringCommand == 0)
            {
                if (!hal_SpiFlashWrite((uint32_t)cmds[i].flashAddr,
                                       cmds[i].ramAddr, cmds[i].size))
                {
                    errorDuringCommand = 1;
                    cmds[i].cmd = FPC_ERROR; //Standard error
                }
                else
                {
                    // Flash write was ok, write magic number.
                    for (cpt = 0; flashSpecialSector[cpt].magicNumber != 0x00000000; ++cpt)
                    {
                        if (cmds[i].flashAddr == flashSpecialSector[cpt].address)
                        {
                            sectorWrite |= 1 << cpt;
                        }
                    }
                }
            }

            // ERROR MANAGEMENT FOR FPC_PROGRAM
            if (errorDuringCommand)
            {
                // programm Error
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
            }
            else
            {
                // Set command is done, switch to next command
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            break;

        case FPC_ERASE_SECTOR:
        {
            uint32_t flashAddress = (uint32_t)cmds[i].flashAddr;
            uint32_t Start, End;
            memd_FlashGetSectorLimits(flashAddress, &Start, &End);
            if (hal_SpiFlashErase(flashAddress, End - Start))
            {
                // OK
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            else
            {
                // Error
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
            }
            break;
        }

        case FPC_ERASE_CHIP:
            // Not implemented.
            cmds[i].cmd = FPC_ERROR;
            SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
            break;

        case FPC_GET_FINALIZE_INFO:
            // Count the number of sectors we have in the flashSpecialSector.
            cpt2 = 0;
            while (flashSpecialSector[cpt2].magicNumber != 0)
                cpt2++;

            // Write into ram all the magic numbers and their addresses
            verifySectorAddress = (UINT32 *)HAL_SYS_GET_UNCACHED_ADDR(cmds[i].ramAddr);
            for (cpt = 0; cpt < cpt2; cpt++)
            {
                *verifySectorAddress = (UINT32)flashSpecialSector[cpt].address;
                verifySectorAddress++;
                *verifySectorAddress = flashSpecialSector[cpt].magicNumber;
                verifySectorAddress++;
            }

            // Inform PC that the command has been treated
            cmds[i].size = cpt2;
            cmds[i].cmd = FPC_DONE;
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            i ^= 1;
            break;

        case FPC_CHECK_FCS:
            s = 0;
            verifySectorAddress = (UINT32 *)HAL_SYS_GET_UNCACHED_ADDR(cmds[i].ramAddr);
            verifySectorSize = (UINT32 *)HAL_SYS_GET_UNCACHED_ADDR(cmds[i].ramAddr + 4);
            verifyAwaitedFcs = (UINT32 *)HAL_SYS_GET_UNCACHED_ADDR(cmds[i].ramAddr + 8);
            verifyFirstBadSector = 0xFFFFFFFF;

            for (cpt = 0; cpt < cmds[i].size; cpt++)
            {
                fcsCtx.srcAddr = (UINT8 *)hal_SpiFlashMapUncacheAddress(*verifySectorAddress);
                fcsCtx.transferSize = *verifySectorSize;
                fcsCtx.userHandler = NULL;

                hal_DmaFcsStart(&fcsCtx);
                hal_DmaReadFcs(fcs);

                if (*verifyAwaitedFcs != (fcs[0] << 16 | (fcs[1] << 8) | fcs[2]))
                {
                    // Uh, oh. Memcmp failed, there's an error.
                    s++;
                    // Replace FCS in ram for this block by the bad fcs
                    *verifyAwaitedFcs = (fcs[0] << 16 | (fcs[1] << 8) | fcs[2]);
                    // Memorize first bad sector
                    if (verifyFirstBadSector == 0xFFFFFFFF)
                    {
                        verifyFirstBadSector = cpt;
                    }
                }
                verifySectorAddress += 3;
                verifySectorSize += 3;
                verifyAwaitedFcs += 3;
            }

            if (s != 0)
            {
                //Send error code.
                cmds[i].ramAddr = (UINT8 *)verifyFirstBadSector;
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
            }
            else
            {
                // No error, write the done flag and pass to the other command
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            break;

        case FPC_CHECK_CALIBRFPART_CRC:
            fcsCtx.srcAddr = (UINT8 *)(((UINT32)(cmds[i].flashAddr)) | 0x80000000); // calibration sector address
            hal_HstSendEvent(fcsCtx.srcAddr[3] << 24 | fcsCtx.srcAddr[2] << 16 | fcsCtx.srcAddr[1] << 8 | fcsCtx.srcAddr[0]);
            if (fcsCtx.srcAddr[3] == 0xCA && fcsCtx.srcAddr[2] == 0x1B)
            {
                if ((fcsCtx.srcAddr[1] << 8 | fcsCtx.srcAddr[0]) > 0x0203)
                {
                    m_CalibRfCrcAdd = 0x1cf8;     // calibration sector size 0x1cfc
                    fcsCtx.transferSize = 0x1360; // The length of RF calibration data
                    fcsCtx.userHandler = NULL;

                    hal_DmaFcsStart(&fcsCtx);
                    hal_DmaReadFcs(fcs);

                    if ((fcs[0] == fcsCtx.srcAddr[m_CalibRfCrcAdd]) &&
                        (fcs[1] == fcsCtx.srcAddr[m_CalibRfCrcAdd + 1]) &&
                        (fcs[2] == fcsCtx.srcAddr[m_CalibRfCrcAdd + 2]))
                    {
                        cmds[i].cmd = FPC_DONE;
                        SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                        i ^= 1;
                    }
                    else
                    {
                        cmds[i].cmd = FPC_DONE;
                        SEND_EVENT(EVENT_CALIB_RFCRC_ERROR + i);
                    }
                }
                else
                { // for low calibration version
                    cmds[i].cmd = FPC_DONE;
                    SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                    i ^= 1;
                }
            }
            else
            {
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_CALIB_RFCRC_ERROR + i);
            }
            break;

        case FPC_END:
            status = MEMD_ERR_NO;
            for (cpt = 0; sectorWrite >> cpt; ++cpt)
            {
                if (sectorWrite & (1 << cpt))
                {
                    if (!hal_SpiFlashWrite((uint32_t)flashSpecialSector[cpt].address,
                                           &flashSpecialSector[cpt].magicNumber,
                                           sizeof(UINT32)))
                    {
                        status = MEMD_ERR_WRITE;
                        break;
                    }

                    // Check if the write of the magic number succeeded
                    void *globalAddress = hal_SpiFlashMapUncacheAddress((uint32_t)flashSpecialSector[cpt].address);
                    if (*((UINT32 *)globalAddress) != flashSpecialSector[cpt].magicNumber)
                    {
                        status = -127; // FIXME: A bit permissive.
                        break;
                    }
                }
            }

            if (status == MEMD_ERR_NO)
            {
                // OK
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;

#if (CHIP_HAS_USB == 1) && defined(FASTPF_USE_USB)
                pmd_SetLevel(PMD_LEVEL_LCD, 1);
                pmd_SetLevel(PMD_LEVEL_KEYPAD, 1);
#endif
            }
            else
            {
                // programm Error
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
#ifdef DEBUG_EVENTS
                hal_HstSendEvent(0xef00000 - status);
#endif
            }
            sectorWrite = 0;
            break;

        case FPC_ERASE_PROGRAM:
            if (FpcEraseProgram(cmds[i].flashAddr, cmds[i].size, cmds[i].ramAddr, cmds[i].fcs))
            {
                sectorWrite |= FpcSpecialSector(cmds[i].flashAddr, flashSpecialSector);

                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            else
            {
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
                i ^= 1;
            }
            break;

        case FPC_ERASE:
            if (FpcErase(cmds[i].flashAddr, cmds[i].size))
            {
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            else
            {
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
                i ^= 1;
            }
            break;

        case FPC_OVERWRITE:
            if (FpcReplace(cmds[i].flashAddr, cmds[i].size, cmds[i].ramAddr))
            {
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            else
            {
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
                i ^= 1;
            }
            break;

        case FPC_LIGHT_ON:
            pmd_SetLevel(PMD_LEVEL_LCD, 7);
            pmd_SetLevel(PMD_LEVEL_KEYPAD, 7);
            pmd_EnablePower(PMD_POWER_CAMERA_FLASH, TRUE);
            cmds[i].cmd = FPC_DONE;
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            i ^= 1;
            break;

        case FPC_FACTORY_READ:
            FpcFactoryRead(cmds[i].flashAddr, cmds[i].ramAddr);
            cmds[i].cmd = FPC_DONE;
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            i ^= 1;
            break;

        case FPC_FACTORY_WRITE:
            if (FpcFactoryWrite(cmds[i].flashAddr, cmds[i].ramAddr))
            {
                cmds[i].cmd = FPC_DONE;
                SEND_EVENT(EVENT_FLASH_PROG_READY + i);
                i ^= 1;
            }
            else
            {
                cmds[i].cmd = FPC_ERROR;
                SEND_EVENT(EVENT_FLASH_PROG_ERROR + i);
                i ^= 1;
            }
            break;

        case FPC_RESTART:
            cmds[i].cmd = FPC_DONE;
            SEND_EVENT(EVENT_FLASH_PROG_READY + i);
            // Sleep a bit to wait for a few ms to be sure the event has been sent.
            hal_HstWaitLastEventSent();

            // Wait 3 32kHz ticks to let the event some distance ahead
            // before unleashing the dogs.
            hal_TimDelay(3);

            // Restart the power cycle the USB Phy and restart the system.
            hal_SysRestart();
            break;
        default:
            cmds[i].cmd = FPC_ERROR;
            SEND_EVENT(EVENT_FLASH_PROG_UNKNOWN + i);
#ifdef DEBUG_EVENTS
            hal_HstSendEvent(0xeee0000);
#endif
            break;
        }
    }
}
