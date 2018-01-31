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

/* bootloader on flash link script */

#undef mips
#define mips mips
OUTPUT_ARCH(mips)
ENTRY(bl_entry)

#define ROTEXT() .text .text.* .gnu.linkonce.t.*
#define RODATA() .rdata .rodata .rodata.* .gnu.linkonce.r.*
#define DATA() .data .data.* .gnu.linkonce.d.*
#define BSS() .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .gnu.linkonce.b.*

#define MEM_CACHED_UNCACHED_OFFSET 0x20000000
#define SRAM_USE_SIZE 0x2000

MEMORY {
    flash (rx): ORIGIN = FLASH_BASE, LENGTH = FLASH_BOOT_OFFSET
    sram (w): ORIGIN = SYS_SRAM_AVAILABLE_BASE, LENGTH = SYS_SRAM_SIZE - (SYS_SRAM_AVAILABLE_BASE - SYS_SRAM_BASE)
}

SECTIONS {
    /* ============================= */
    /* bootloader entry in flash     */
    /* ============================= */
    .entry : ALIGN(0x10) {
        LONG(0xFFFFFFFF) /* LONG(0xD9EF0045) */
        . = ALIGN(0x10);

        *(.bl_entry)
    } > flash

    /* ============================= */
    /* .fixptr                       */
    /* ============================= */
    . = ORIGIN(sram) - 4 + MEM_CACHED_UNCACHED_OFFSET;
    .fixptr (NOLOAD) : {
        KEEP(*(.fixptr))
    }

    __sram_heap_start = ORIGIN(sram);
    __sram_heap_end = ORIGIN(sram) + LENGTH(sram) - SRAM_USE_SIZE;
    __sram_top = ORIGIN(sram) + LENGTH(sram);

    /* ============================= */
    /* code in sys sram              */
    /* ============================= */
    . = __sram_heap_end;
    .sram_text ALIGN(0x10) : {
        __sram_text_start = .;

        *bl_spi_flash.*(ROTEXT() RODATA())
        *(.sramtext)

        __sram_text_end = .;
    } AT>flash

    __sram_text_load_start = LOADADDR(.sram_text);

    /* ============================= */
    /* data in sys sram              */
    /* ============================= */
    .sram_data ALIGN(0x10) : {
        __sram_data_start = .;

        *(DATA())
        *(.sramdata)

        __sram_data_end = .;
    } AT>flash

    __sram_data_load_start = LOADADDR(.sram_data);

    /* ============================= */
    /* uncached data in sys sram     */
    /* ============================= */
    . += MEM_CACHED_UNCACHED_OFFSET;
    .sram_ucdata ALIGN(0x10) : {
        __sram_ucdata_start = .;

        *(.sramucdata)

        __sram_ucdata_end = .;
    } AT>flash
    . -= MEM_CACHED_UNCACHED_OFFSET;

    __sram_ucdata_load_start = LOADADDR(.sram_ucdata);

    /* ============================= */
    /* bss in sys sram               */
    /* ============================= */
    .sram_bss ALIGN(0x10) : {
        __sram_bss_start = .;

        *(BSS())
        *(.srambss)

        __sram_bss_end = .;
    }

    /* ============================= */
    /* uncached bss in sys sram      */
    /* ============================= */
    . += MEM_CACHED_UNCACHED_OFFSET;
    .sram_ucbss ALIGN(0x10) : {
        __sram_ucbss_start = .;
        *(.sramucbss)
        __sram_ucbss_end = .;
    }
    . -= MEM_CACHED_UNCACHED_OFFSET;

    /* ============================= */
    /* code in flash                 */
    /* ============================= */
    .flash_text : ALIGN(0x10) {
        *(ROTEXT() RODATA())
    } > flash
}
