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
#include "hal_ebc.h"
#include "hal_sys.h"
#include "string.h"
#include "memd_m.h"
#include "memdp_debug.h"
// #include "hal_host.h"
// #include "sys_ctrl.h"
#include "global_macros.h"
#include "mem_cmd.h"
#include "hal_spi_flash.h"

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
extern CONST MEMD_FLASH_LAYOUT_T g_memdFlashLayout;

UINT32 g_memdFlashBaseAddress = 0;
MEMD_SPIFLSH_RDID_T HAL_BOOT_DATA_INTERNAL g_spiflash_ID;
PRIVATE UINT8 HAL_UNCACHED_DATA_INTERNAL g_spiflash_enble_suspend = 0;

#ifdef __VDS_QUICK_FLUSH__
PUBLIC BOOL memd_SuspendIsEnable(VOID);
#endif

#if !defined(CHIP_DIE_8955) && !defined(CHIP_DIE_8909)
PRIVATE VOID HAL_FUNC_INTERNAL memd_RamConfigureBurstMode(REG16 *csBase, REG32 *pCRE)
{
    UINT16 read;
    // Using the CR controlled method
    // Configure BCR
    // PSRAM_BCR[15] : 0 = synch / 1 = asynch
    // PSRAM_BCR[14] : 0 = variable Latency / 1 = Fixed latency
    // PSRAM_BCR[13:11] : Latency count 011(3)-111(7) clock cycle could be used
    // PSRAM_BCR[10] : Wait Polarity 0 = Active Low / 1 = Active High
    // PSRAM_BCR[9] : MUST be set to 0
    // PSRAM_BCR[8] : Wait Config 0 = Asserted during wait / 1 = Asserted 1 cycle before wait
    // PSRAM_BCR[7:6] : MUST be set to 0
    // PSRAM_BCR[5:4] : driver strength bits 0 = Full Drive / 1 = 1/2 Drive / 2 = 1/4 Drive
    // PSRAM_BCR[3] : 0 = Wrap / 1 = No wrap
    // PSRAM_BCR[2:0] : Burst len 001 = 4 words / 010 = 8 words / 011 = 16 words / 100 = 32 words / 111 Continuous
    read = *(csBase + PSRAM_BCR);
    *pCRE = 1;                    // MEM_FIFO_CRE
    *(csBase + PSRAM_BCR) = read; // read burst with latency = 3 fixed (4 clocks)

    {
        UINT32 flush __attribute__((unused)) = *pCRE;
    }

    *pCRE = 0;
#ifdef CHIP_DIE_8809E2
    UINT32 flush __attribute__((unused));
#define UNCACHED(n) (UINT16 *)(((UINT32)n) | 0xa0000000)
    *pCRE = 1;
    do
    {
        read = *UNCACHED((csBase + PSRAM_BCR));
        flush = *pCRE;
    } while (read != (PSRAM_BCR & 0xffff));
    *pCRE = 0;
#endif
}

// =============================================================================
// memd_RamOpen
// -----------------------------------------------------------------------------
/// That function configures the RAM.
/// It must be called to configure the best settings for the RAM, thus enabling
/// the fatest access.
/// @param cfg Ram configuration used to open the RAM.
///
// =============================================================================
PUBLIC VOID MEMD_BOOT_FUNC memd_RamOpen(CONST MEMD_RAM_CONFIG_T *cfg)
{
    MEMD_ASSERT(cfg != NULL, "Opening RAM with a NULL configuration");
// Open External SRAM
#ifdef MEMD_RAM_IS_BURST
    hal_EbcSramOpen(&cfg->csConfig, memd_RamConfigureBurstMode);
#else  //!MEMD_RAM_IS_BURST
    hal_EbcCsOpen(HAL_EBC_SRAM, &cfg->csConfig);
    dfs
#endif //!MEMD_RAM_IS_BURST

/*
    [16 14...13]size: 0000: 8Mbit     0001: 16Mbit     0010: 32Mbit      0011: 64Mbit
                    1000:128Mbit     1001:256Mbit
    [12]chip num: 0: 1 chip            1: 2 chip
    */

#if defined(CHIP_DIE_8809E2) || defined(CHIP_DIE_8955) || defined(CHIP_DIE_8909)
#else
        *(volatile unsigned int *)ADDR_SAVING_FLASHID_4_USB = FLASH_1_32MBIT;
#endif
}
#endif

// =============================================================================
// memd_FlashErase_Respond_Irq
// -----------------------------------------------------------------------------
/// This function dont block the sys when erase flash.
/// Addresses <B> must be aligned on sectors</B>:
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
PUBLIC MEMD_ERR_T memd_FlashErase_Respond_Irq(UINT8 *startFlashAddress, UINT8 *endFlashAddress, UINT32 time)
{
    UINT32 Start, End;
    UINT32 phys_end_add, phys_start_add;

    phys_start_add = (UINT32)startFlashAddress;

    MEMD_ASSERT(phys_start_add < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");

    MEMD_ASSERT((phys_start_add & 0xe0000000) == 0,
                "flash_address is expected to be a byte offset within the flash chip, not an absolute adresse");

    // Check that start & end addresses are aligned
    if (endFlashAddress == NULL)
    {
        memd_FlashGetSectorLimits(phys_start_add, &Start, &End);
        phys_end_add = End;
    }
    else
    {
        phys_end_add = (UINT32)endFlashAddress;
        MEMD_ASSERT((phys_end_add & 0xe0000000) == 0,
                    "flash_address is expected to be a byte offset within the flash chip, not an absolute adresse");
        if (phys_end_add != FLASH_SIZE)
        {
            memd_FlashGetSectorLimits(phys_end_add, &Start, &End);
            if (phys_end_add != Start)
            {
                MEMD_ASSERT(0, "flash erase error 1,phys_end_add = 0x%x,start = 0x%x.", phys_end_add, Start);
                return MEMD_ERR_ALIGN;
            }
        }
    }

    memd_FlashGetSectorLimits(phys_start_add, &Start, &End);
    if (phys_start_add != Start)
    {
        MEMD_ASSERT(0, "flash erase error 2,phys_end_add = 0x%x,start = 0x%x.", phys_end_add, Start);
        return MEMD_ERR_ALIGN;
    }

    while (Start != phys_end_add)
    {
        if (!hal_SpiFlashErase(Start, End - Start))
            return MEMD_ERR_ERASE;
        if (End != FLASH_SIZE)
            memd_FlashGetSectorLimits(End, &Start, &End);
        else
            Start = End;
    }
    return MEMD_ERR_NO;
}

// =============================================================================
// memd_FlashErase
// -----------------------------------------------------------------------------
/// This function erases contiguous flash sectors.
/// Addresses <B> must be aligned on sectors</B>:
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
PUBLIC MEMD_ERR_T memd_FlashErase(UINT8 *startFlashAddress, UINT8 *endFlashAddress)
{
    UINT32 Start, End;
    UINT32 phys_end_add, phys_start_add;
    UINT32 u_TempSectorSize;

    phys_start_add = (UINT32)startFlashAddress;

    MEMD_ASSERT(phys_start_add < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");

    MEMD_ASSERT((phys_start_add & 0xe0000000) == 0,
                "flash_address is expected to be a byte offset within the flash chip, not an absolute adresse");

    // Check that start & end addresses are aligned
    if (endFlashAddress == NULL)
    {
        memd_FlashGetSectorLimits(phys_start_add, &Start, &End);
        phys_end_add = End;
    }
    else
    {
        phys_end_add = (UINT32)endFlashAddress;
        MEMD_ASSERT((phys_end_add & 0xe0000000) == 0,
                    "flash_address is expected to be a byte offset within the flash chip, not an absolute adresse");
        if (phys_end_add != FLASH_SIZE)
        {
            memd_FlashGetSectorLimits(phys_end_add, &Start, &End);
            if (phys_end_add != Start)
            {
                MEMD_ASSERT(0, "flash erase error 1,phys_end_add = 0x%x,start = 0x%x.", phys_end_add, Start);
                return MEMD_ERR_ALIGN;
            }
        }
    }

    u_TempSectorSize = memd_FlashGetSectorLimits(phys_start_add, &Start, &End);
    if (phys_start_add != Start)
    {
        MEMD_ASSERT(0, "flash erase error 2,phys_end_add = 0x%x,start = 0x%x.", phys_end_add, Start);
        return MEMD_ERR_ALIGN;
    }

    while (Start != phys_end_add)
    {
        if (!hal_SpiFlashErase(Start, End - Start))
            return MEMD_ERR_ERASE;
        if (End != FLASH_SIZE)
            memd_FlashGetSectorLimits(End, &Start, &End);
        else
            Start = End;
    }
    return MEMD_ERR_NO;
}

// ============================= memd_FlashErase2 ================================================
PUBLIC MEMD_ERR_T memd_FlashEraseNOSectorLimit(
    UINT8 *startFlashAddress,
    UINT8 *endFlashAddress)
{
    UINT32 start;
    UINT32 end;
    UINT32 size;

    start = (UINT32)startFlashAddress;
    end = (UINT32)endFlashAddress;
    if (end < start)
        return MEMD_ERR_ALIGN;

    size = end - start;
    if ((start & 0xfff) != 0 || (size & 0xfff) != 0)
        return MEMD_ERR_ALIGN;

    if (!hal_SpiFlashErase(start, end - start))
        return MEMD_ERR_ERASE;
    return MEMD_ERR_NO;
}

PUBLIC MEMD_ERR_T memd_FlashWrite(UINT8 *flashAddress,
                                  UINT32 byteSize,
                                  UINT32 *pWrittenByteSize,
                                  CONST UINT8 *buffer)
{
    MEMD_ASSERT((UINT32)flashAddress < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");

    if (!hal_SpiFlashWrite((UINT32)flashAddress, buffer, byteSize))
        return MEMD_ERR_WRITE;
    if (pWrittenByteSize != NULL)
        *pWrittenByteSize = byteSize;
    return MEMD_ERR_NO;
}

// =============================================================================
// memd_FlashWrite_Respond_Irq
// -----------------------------------------------------------------------------
/// This function dont block the sys when write flash .
/// Addresses <B> must be aligned on sectors</B>:
/// - The \c startFlashAddress is the address of the first sector to erase.
/// @param start_flashAddress The address of the first sector to erase
/// @return #MEMD_ERR_NO, #MEMD_ERR_ERASE, #MEMD_ERR_ALIGN or #MEMD_ERR_PROTECT
/// whether the operation succeeded or failed
/// memd_FlashWrite_Respond_Irq
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashWrite_Respond_Irq(UINT8 *flashAddress,
                                              UINT32 byteSize,
                                              UINT32 *pWrittenByteSize,
                                              UINT8 *buffer)
{
    MEMD_ASSERT((UINT32)flashAddress < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");

    if (!hal_SpiFlashWrite((UINT32)flashAddress, buffer, byteSize))
        return MEMD_ERR_WRITE;
    if (pWrittenByteSize != NULL)
        *pWrittenByteSize = byteSize;
    return MEMD_ERR_NO;
}

/* get flash id
> w 0xa1a25000,6
> w 0xa1a25008,0x12
> w 0xa1a25008,0x12
> w 0xa1a25008,0x34
> w 0xa1a25008,0x56
> w 0xa1a25008,0x78
> w 0xa1a25000,0x10002
*/

PUBLIC MEMD_FLASH_LAYOUT_T *memd_FlashOpen(CONST MEMD_FLASH_CONFIG_T *cfg)
{
    hal_SpiFlashOpen();
    g_memdFlashBaseAddress = (UINT32)hal_EbcFlashOpen(HAL_SYS_FREQ_39M, &cfg->csConfig, NULL);

    uint32_t id = hal_SpiFlashGetID();
    g_spiflash_ID.manufacturerID = id & 0xff;
    g_spiflash_ID.device_memory_type_ID = (id >> 8) & 0xff;
    g_spiflash_ID.device_capacity_type_ID = (id >> 16) & 0xff;
    return ((MEMD_FLASH_LAYOUT_T *)&g_memdFlashLayout);
}

// =============================================================================
// memd_FlashRead
// =============================================================================
PUBLIC MEMD_ERR_T memd_FlashRead(UINT8 *flashAddress, UINT32 byteSize, UINT32 *pReadByteSize, UINT8 *buffer)
{
    VOLATILE UINT8 *ptr;
    UINT32 phys_add = (UINT32)flashAddress;

    MEMD_ASSERT(phys_add < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");
    MEMD_ASSERT((phys_add & 0xe0000000) == 0,
                "flash_address is expected to be a byte offset within the flash chip, not an absolute adresse");

    ptr = (VOLATILE UINT8 *)(g_memdFlashBaseAddress + phys_add);
    /* could do aligned read from flash to improve bus accesses as it is uncached */
    memcpy(buffer, (UINT8 *)ptr, byteSize);
    *pReadByteSize = byteSize;
    return MEMD_ERR_NO;
}

PUBLIC VOLATILE CONST UINT8 *memd_FlashGetGlobalAddress(UINT8 *flashAddress)
{
    VOLATILE CONST UINT8 *ptr;
    UINT32 phys_add;
    phys_add = (UINT32)flashAddress;
    MEMD_ASSERT(phys_add < FLASH_SIZE,
                "flash_address is out of the flash chip, not an valid adresse");
    ptr = (VOLATILE CONST UINT8 *)(g_memdFlashBaseAddress + phys_add);
    return ptr;
}

PUBLIC UINT32 memd_FlashGetSectorLimits(UINT32 sectorId, UINT32 *start, UINT32 *end)
{
    UINT32 aStart, aEnd;
    int bankNum = 0;
    int majSecIdx, minSecIdx;
    UINT32 sectSize;

    if (sectorId == g_memdFlashLayout.fullSize)
    {
        *start = g_memdFlashLayout.fullSize;
        *end = g_memdFlashLayout.fullSize;
        return 0;
    }
    else if (sectorId > g_memdFlashLayout.fullSize)
    {
        *start = 0;
        *end = 0;
        MEMD_ASSERT(FALSE, "Out of accessible flash space !!!!");
        return 0;
    }
    else
    {
        while ((sectorId >= g_memdFlashLayout.bankLayout[bankNum + 1].bankStartAddr) &&
               (bankNum < (g_memdFlashLayout.numberOfBank - 1)))
        {
            ++bankNum;
        }

        aStart = g_memdFlashLayout.bankLayout[bankNum].bankStartAddr;
        majSecIdx = 0;
        aEnd = aStart +
               (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);

        while (sectorId >= aEnd)
        {
            if (majSecIdx == 2)
            {
                *start = 0;
                *end = 0;
                MEMD_ASSERT(FALSE, "Out of accessible flash space !!!!");
                return 0;
            }
            else
            {
                ++majSecIdx;
                aStart = aEnd;
                aEnd = aStart +
                       (g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][0] *
                        g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1]);
            }
        }

        minSecIdx = 0;
        sectSize = g_memdFlashLayout.bankLayout[bankNum].sectorLayoutTable[majSecIdx][1];
        aEnd = aStart + sectSize;

        while (sectorId >= aEnd)
        {
            aStart = aEnd;
            aEnd = aStart + sectSize;
            ++minSecIdx;
        }

        if ((sectorId % 0x10000) != 0)
        {
            sectSize = 0x1000;
            *start = sectorId & (~0xfff);
            *end = (sectorId & (~0xfff)) + sectSize;
        }
        else
        {
            *start = aStart;
            *end = aEnd;
        }

        return sectSize;
    }
}

#ifdef __VDS_QUICK_FLUSH__

VOID memd_SetFramsIrqIgnore(BOOL Ignore)
{
}

BOOL memd_SysIsIdle(void)
{
    return TRUE;
}

#endif

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
PUBLIC VOID memd_FlashGetUserFieldInfo(UINT32 *userFieldBase, UINT32 *userFieldSize, UINT32 *sectorSize, UINT32 *blockSize)
{
    UINT32 Start, End;
    *userFieldBase = g_memdFlashLayout.userDataBase;
    *userFieldSize = g_memdFlashLayout.userDataSize;
    *sectorSize = memd_FlashGetSectorLimits(*userFieldBase, &Start, &End);
    *blockSize = g_memdFlashLayout.userDataSectorLogicalBlockSize;
}

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
PUBLIC VOID HAL_FUNC_INTERNAL memd_FlashSleep(VOID)
{
    hal_SpiFlashSleep();
}

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
PUBLIC VOID HAL_FUNC_INTERNAL memd_FlashWakeup(VOID)
{
    hal_SpiFlashWakeup();
}

PUBLIC VOID memd_FlashRst(VOID)
{
    hal_SpiFlashReset();
}

PUBLIC UINT32 memd_FlashErase2SuspendTime(VOID)
{
    return 0;
}

PUBLIC VOID memd_SuspendEnable(VOID)
{
    g_spiflash_enble_suspend = 1;
}

PUBLIC VOID memd_SuspendDisable(VOID)
{
    g_spiflash_enble_suspend = 0;
}

PUBLIC BOOL memd_SuspendIsEnable(VOID)
{
    return g_spiflash_enble_suspend == 1 ? TRUE : FALSE;
}

PUBLIC void memd_Flash_CalibWriteFlashFlag(BOOL writeFlag)
{
}

/*protect_section: 0:not protect 1:0~0xffff 2:0~0x1ffff 3:0~0x3ffff 4:0~0x7ffff 5:0~0xfffff*/
/*otp_val: 0:not do otp  1:do otp*/
/*return val: 0:success 1:could not do otp -1:parameter error */
PUBLIC int memd_FlashCodeSectionOtp(UINT8 protect_section, BOOL otp_val)
{
    UINT16 reg_status;

    if (protect_section > 5)
        return -1;
    if (protect_section)
        protect_section += 0x8;

    reg_status = hal_SpiFlashReadStatusRegister();
    if (reg_status & 0x0180)
        return 1;

    UINT16 mask = 0x0180 | 0x7c;
    UINT16 value = 0x0180 | (protect_section << 2);
    hal_SpiFlashWriteStatusRegister(value, mask);
    return 0;
}

PUBLIC MEMD_ERR_T memd_Flash_security_REG_Erase(UINT8 *startFlashAddress, UINT8 *endFlashAddress)
{
    UINT8 addr_tmp = ((UINT32)startFlashAddress >> 12) & 3;
    UINT8 addr_tmp_end = ((UINT32)endFlashAddress >> 12) & 3;
    MEMD_ERR_T status = MEMD_ERR_NO;
    UINT32 temp_erase_addr = (UINT32)startFlashAddress;

    if (endFlashAddress == NULL)
        addr_tmp_end = addr_tmp;

    if (addr_tmp <= 0 || addr_tmp > 3)
        return MEMD_ERR_ERASE;
    if (addr_tmp_end <= 0 || addr_tmp_end > 3)
        return MEMD_ERR_ERASE;
    if (addr_tmp > addr_tmp_end)
        return MEMD_ERR_ERASE;

    while (addr_tmp <= addr_tmp_end)
    {
        hal_SpiFlashSecurityErase(temp_erase_addr);
        addr_tmp += 1;
        temp_erase_addr += 0x1000;
    }
    return status;
}

PUBLIC MEMD_ERR_T memd_Flash_security_REG_Read(UINT8 *flashAddress, UINT32 byteSize, UINT8 *buffer)
{
    hal_SpiFlashSecurityRead((UINT32)flashAddress, buffer, byteSize);
    return MEMD_ERR_NO;
}

PUBLIC MEMD_ERR_T memd_Flash_security_REG_Write(UINT8 *flashAddress,
                                                UINT32 byteSize,
                                                UINT32 *pWrittenByteSize,
                                                CONST UINT8 *buffer)
{
    if (byteSize > 256)
        return MEMD_ERR_WRITE;

    hal_SpiFlashSecurityWrite((UINT32)flashAddress, buffer, byteSize);
    if (pWrittenByteSize != NULL)
        *pWrittenByteSize = byteSize;
    return MEMD_ERR_NO;
}

PUBLIC VOID memd_Flash_security_REG_Lock(UINT8 *flashAddress)
{
    UINT32 flash_addr = (UINT32)flashAddress;
    UINT16 mask = 0;

    if (((flash_addr >> 12) & 0x3) == 1)
        mask = 0x0800;
    else if (((flash_addr >> 12) & 0x3) == 2)
        mask = 0x1000;
    else if (((flash_addr >> 12) & 0x3) == 3)
        mask = 0x2000;

    hal_SpiFlashWriteStatusRegister(0xffff, mask);
}
