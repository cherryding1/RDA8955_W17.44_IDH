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

#include "bl_config.h"
#include "bl_platform.h"
#include "bl_spi_flash.h"
#include "bl_entry.h"
#include "bl_malloc.h"
#include "bl_log.h"
#include "boot_host.h"
#include <stdint.h>
#include <stdbool.h>

// BOOTLOADER SRAM USAGE
//
// There are 2 kinds of LOD: FLASH and RAMRUN. Typical usage of FLASH is
// the blfota, and typical usage of RAMRUN is blfpc.
//
// FLASH: If bootloader will jump to second LOD, it is needed to access
// SRAM as little as possible. It is possible that some information has
// already written into SRAM, and second LOD will use it. So, bootloader
// will use the last 8KB for text/data/bss. When it is known not to jump
// to second LOD, other SRAM can be used as heap.
//
// RAMRUN: All SRAM can be used. And reserved 8K for stack.

typedef void (*BOOT_SECTOR_T)(uint32_t param);
#define BOOT_SECTOR_PATTERN_ADDRESS BL_BOOT_SECTOR_ADDRESS
#define BOOT_SECTOR_ENTRY_POINT (BL_BOOT_SECTOR_ADDRESS + 0x10)
#define BOOT_FLASH_PROGRAMMED_PATTERN 0xD9EF0045

void bl_rt_init(void)
{
    extern char __sram_text_start;
    extern char __sram_text_end;
    extern char __sram_text_load_start;
    extern char __sram_data_start;
    extern char __sram_data_end;
    extern char __sram_data_load_start;
    extern char __sram_ucdata_start;
    extern char __sram_ucdata_end;
    extern char __sram_ucdata_load_start;
    extern char __sram_bss_start;
    extern char __sram_bss_end;
    extern char __sram_ucbss_start;
    extern char __sram_ucbss_end;
    extern char __sram_heap_start;
    extern char __sram_heap_end;

#define NAME_SIZE(name) (&__##name##_end - &__##name##_start)

#define LOAD_DATA(name)                                                          \
    do                                                                           \
    {                                                                            \
        if (&__##name##_start != &__##name##_load_start && NAME_SIZE(name) != 0) \
            memcpy(&__##name##_start, &__##name##_load_start, NAME_SIZE(name));  \
    } while (0)

#define CLEAR_DATA(name)                                   \
    do                                                     \
    {                                                      \
        if (NAME_SIZE(name) != 0)                          \
            memset(&__##name##_start, 0, NAME_SIZE(name)); \
    } while (0)

    LOAD_DATA(sram_text);
    LOAD_DATA(sram_data);
    LOAD_DATA(sram_ucdata);

    CLEAR_DATA(sram_bss);
    CLEAR_DATA(sram_ucbss);

    bl_dcache_wb_all();

    bl_heap_init((uint32_t *)&__sram_heap_start, NAME_SIZE(sram_heap) / sizeof(uint32_t),
                 (uint32_t *)0x82000000, 0x400000 / sizeof(uint32_t));

#undef NAME_SIZE
#undef LOAD_DATA
#undef CLEAR_DATA
}

void bl_init(void)
{
    bl_log_init();
    bl_platform_init();
    bl_SpiFlashOpen();
}

void __attribute__((nomips16, section(".bl_entry"))) bl_entry(uint32_t param)
{
    bl_invalidate_icache();

    bl_rt_init();

    bl_main(param);

    // If bl_main returns, we will boot next boot sector
    if (*(uint32_t *)BOOT_SECTOR_PATTERN_ADDRESS == BOOT_FLASH_PROGRAMMED_PATTERN)
    {
        BOOT_SECTOR_T bootSector = (BOOT_SECTOR_T)BOOT_SECTOR_ENTRY_POINT;
        bl_invalidate_icache(); // no really needed, jsut in case
        bl_log_close(); // most of the 2nd LOD will assert when debughost is open
        bootSector(param);
    }

    // never return
    for (;;)
        ;
}

void bl_run_with_sram_top(uint32_t param, void (*fun)(uint32_t param))
{
    extern char __sram_top[];
    bl_run_with_stack(param, fun, __sram_top - 4);
}

// Copied by the lod command of CoolWatcher. Useful for Ramruns.
BOOT_HST_EXECUTOR_T g_bootHstExecutorCtx POSSIBLY_UNUSED SECTION_EXECUTOR_CTX =
    {{BOOT_HST_MONITOR_X_CMD, (UINT32)bl_entry, 0, 0, 0}, {0}};
