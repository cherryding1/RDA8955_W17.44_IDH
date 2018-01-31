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

#ifndef _BL_PLATFORM_H_
#define _BL_PLATFORM_H_

#include <stdint.h>
#include <stdbool.h>
#include "bl_rom_api.h"

#define SECTION_SRAMTEXT __attribute__((section(".sramtext")))
#define SECTION_SRAM_DATA __attribute__((section(".sramdata")))
#define SECTION_SRAM_UCDATA __attribute__((section(".sramucdata")))
#define SECTION_SRAM_BSS __attribute__((section(".srambss")))
#define SECTION_SRAM_UCBSS __attribute__((section(".sramucbss")))
#define SECTION_FIXPTR __attribute__((section(".fixptr")))
#define SECTION_EXECUTOR_CTX __attribute__((section(".executor_ctx")))

#define UNCACHE_ADDRESS(p) ((void *)((unsigned)(p) | 0x20000000))
#define CACHE_LINE_SIZE (16)
#define CACHE_LINE_MASK (15)
#define CACHE_LINE_ALIGN_UP(p) (((p) + CACHE_LINE_SIZE) & ~CACHE_LINE_MASK)
#define CACHE_LINE_ALIGN_DOWN(p) ((p) & ~CACHE_LINE_MASK)

static inline void bl_panic(void) { asm("break 1"); }
static inline void bl_send_event(unsigned evt) { mon_Event(evt); }
static inline void bl_invalidate_icache(void) { boot_InvalidICache(); }
static inline void bl_dcache_wb_all(void) { boot_FlushDCache(0); }
static inline void bl_dcache_wbinv_all(void) { boot_FlushDCache(1); }

unsigned bl_hw_tick(void);
void bl_platform_init(void);
void bl_platform_reset(void);
void bl_platform_shutdown(void);

void bl_watchdog_start(unsigned second);
void bl_watchdog_reload();
void bl_watchdog_stop();

void bl_delay_us(unsigned us);
void bl_delay_tick(unsigned tick);

void bl_log_init(void);
void bl_log_close(void);

uint16_t bl_ispi_read(uint8_t cs, uint16_t address);
void bl_ispi_write(uint8_t cs, uint16_t address, uint16_t data);

#endif
