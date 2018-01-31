/* ======================================================================= */
/*                  TARGET CONFIGURATION SECTION                           */
/* ======================================================================= */
/* The following values are defined in ff_target.def and exported by the Makefile */
/*  FLASH_SIZE              */
/*  RAM_SIZE                */
/*  CALIB_BASE              */
/*  FACT_SETTINGS_BASE      */
/*  CODE_BASE               */
/*  USER_DATA_BASE          */
/*  USER_DATA_SIZE          */
/* ======================================================================= */
/*                  END CONFIGURATION SECTION                              */
/* ======================================================================= */
/* The linker script depends on the following defines */
/*  - CONST_IN_RAM : all constants are placed in RAM, this is useful for ram_run */
/*  - CODE_IN_RAM : All code is executed from RAM, this is useful for ram_run or code running from RAM on romulator */
/*  - PRELOADED_INT_SRAM : The internal SRAM are already preloaded, useful for ram_run */
/*  - PRELOADED_RAM : The external RAM is already preloaded, useful for ram_run or simu */
/* Those are constants of the system */


/* ROM content selection: things to include
    WARNING: the DISCARD section uses the opposite -->
    do all inclusion before discard !*/


/* Take into account assert bug in binutils-2.15*/
#if (USE_BINUTILS_2_19 == 1)
#define LD_ASSERT(condition, small, full)   ASSERT(condition, full)
#else
#define LD_ASSERT(condition, small, full)   ASSERT(condition, small)
#endif

#define MEM_CACHED_UNCACHED_OFFSET 0x20000000

#define FILENAME_PREFIX(pattern, sections) \
    /* Files in library */ \
    *:pattern(sections) \
    /* Files with path */ \
    */pattern(sections)

#define ROTEXT() .text .text.* .gnu.linkonce.t.*
#define RODATA() .rdata .rodata .rodata.* .gnu.linkonce.r.*
#define DATA() .data .data.* .gnu.linkonce.d.*
#define BSS() .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*

#define BOOT_BANNED_SECTIONS() .sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss DATA() BSS()


ENTRY (boot_Sector)
/*EXTERN (boot_Sector)*/


SECTIONS
{
    .check_boot_banned_sections :
    {
        FILENAME_PREFIX(boot_*.o, BOOT_BANNED_SECTIONS())
        FILENAME_PREFIX(target_boot_config.o, BOOT_BANNED_SECTIONS())
        FILENAME_PREFIX(drv_ram.o, BOOT_BANNED_SECTIONS())
        /* For backward compatibility, some object files are allowed to
         * have normal data or bss sections, e.g., drv_flsh.o
         */
    }
    LD_ASSERT(SIZEOF(.check_boot_banned_sections) == 0, "BSER", "Boot files cannot contain normal data or bss sections")

#ifdef LOAD_ROM_ELF
    /* ======================================== */
    /* internal rom in flash elf */
    /* ======================================== */
    #include "link_rom_elf.ld"
#endif

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */

    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */

#if defined(RAMRUN) || defined(ASICSIMU)
    /* For ramruns, this section loads an execution context
     * content allowing to directly executed the
     * boot sector (preloaded with PC=boot_Sector and SP=0,
     * cf boot_sector.c) */
    .executor_ctx ROM_MISC_UBASE : AT (ADDR(.executor_ctx) - MEM_CACHED_UNCACHED_OFFSET)
    {
        /* For ramruns, avoid removing .executor_ctx even when option
         * --gc-sections is enabled, as its content must be loaded from
         * the ramrun lod */
        KEEP(*(.executor_ctx))
    }
#endif

    /*----------------- */
    /* SYS SRAM CACHED  */
    /*----------------- */

    LD_ASSERT(ABSOLUTE(__int_rom_reserved_end) <= ROM_MISC_BASE + ROM_MISC_SIZE, "RRRO", "Reserved Ram Reserved for Rom Overflowed")

    . = ROM_MISC_BASE + ROM_MISC_SIZE;

    /* HAL Boot Sector reload structure, contains information written by
     * HAL during the normal execution of the code and used by the
     * Boot Sector to reload the ram image when the phone boots.
     * It is mapped in the Internal SRAM because we don't want the Boot
     * Sector code to access the External RAM. And it is mapped at a fixed
     * location (after the last patch for the chip) because we
     * don't want the RAMRUN to overwrite it.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .hal_boot_sector_reload_struct . (NOLOAD) :
        AT (ADDR(.hal_boot_sector_reload_struct))
    {
        KEEP(*(.hal_boot_sector_reload_struct))
    }

    /* HAL Boot Sector structure, contains information written by
     * HAL during the normal execution of the code and used by the
     * Boot Sector when the phone boots.
     * It is mapped in the Internal SRAM because we don't want the Boot
     * Sector code to access the External RAM. And it is mapped at a fixed
     * location (after the last patch for the chip) because we
     * don't want the RAMRUN to overwrite it.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_struct . (NOLOAD) :
        AT (ADDR(.boot_sector_struct))
    {
        KEEP(*(.boot_sector_struct))
    }

    /* Boot Sector reload structure pointer, used to retrieve the
     * structure filled by HAL and used by the Boot Sector code.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_reload_struct_ptr (SYS_SRAM_AVAILABLE_BASE-4-4-4) (NOLOAD) :
        AT (ADDR(.boot_sector_reload_struct_ptr))
    {
        KEEP(*(.boot_sector_reload_struct_ptr))
    }

    /* Boot Sector structure pointer, used to retrieve the command
     * structure filled by HAL and used by the Boot Sector code.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_struct_ptr (SYS_SRAM_AVAILABLE_BASE-4-4) (NOLOAD) :
        AT (ADDR(.boot_sector_struct_ptr))
    {
        KEEP(*(.boot_sector_struct_ptr))
    }

    /* Fixed pointer, used to retrieve the map structure. */
    .fixptr (SYS_SRAM_AVAILABLE_BASE-4) (NOLOAD) : AT (ADDR(.fixptr))
    {
        KEEP(*(.fixptr))
    }

    . = (SYS_SRAM_BASE) + (SYS_SRAM_SIZE) - (RESERVED_RAMINIT_SIZE) \
        - (SYS_SRAM_IRQ_STACKWATCH_SIZE) - (XCPU_IRQ_STACK_SIZE) - (RESERVED_MBX_SIZE);

    _sys_sram_raminit_start = .;

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = .;

    _boot_sys_sram_start = .;

    _boot_sys_sram_txt_start = .;
    .raminittext . : AT (ADDR(.raminittext))
    {
        _boot_sector = .;
        *(.boot_sector_start)

        *(.bootsramtext)
        *(.bootramtext)
        *(.boottext)
        *(.bootrodata)

        *boot_sector.o(ROTEXT() RODATA())
        *boot_sector_chip.o(ROTEXT() RODATA())
        *boot_sys.o(ROTEXT() RODATA())
        *boot_mem_init.o(ROTEXT() RODATA())
        *boot_sector_driver.o(ROTEXT() RODATA())
        *target_boot_config.o(ROTEXT() RODATA())
        *drv_ram.o(ROTEXT() RODATA())

        . = ALIGN(0x10);
    }
    _boot_sys_sram_txt_end = .;
    _boot_sys_sram_txt_size = _boot_sys_sram_txt_end - _boot_sys_sram_txt_start;

    _boot_sys_sram_data_load_start = .;

#ifdef SYS_SRAM_DATA_SIZE
    . += SYS_SRAM_DATA_SIZE;
#endif

    _boot_sys_sram_data_start = .;
    .raminitdata . : AT (_boot_sys_sram_data_load_start)
    {
        *(.bootsramdata)
        *(.bootdata)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_data_end = .;
    _boot_sys_sram_data_size = _boot_sys_sram_data_end - _boot_sys_sram_data_start;

#if defined(SYS_SRAM_DATA_SIZE) && (SYS_SRAM_DATA_SIZE > 0)
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start + _boot_sys_sram_data_size;

    /* Check that the boot sys sram data is not over overflowed */
    LD_ASSERT(ABSOLUTE(_boot_sys_sram_data_start) >= ABSOLUTE(_boot_sys_sram_data_load_end), "BDOV", "Boot sys sram data overflow (SYS_SRAM_DATA_SIZE too small) !")
#else
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start;
#endif

    _boot_sys_sram_end = .;
    _boot_sys_sram_size = _boot_sys_sram_end - _boot_sys_sram_start;

    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location;

    /* ------------------------------------------------ */
    /* BOOTING BSS IN SYS SRAM (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_sys_sram_bss_start = .;
    .raminitbss . (NOLOAD) : AT (ADDR(.raminitbss))
    {
        *(.bootsrambss)
        *(.bootbss)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_bss_end = .;
    _boot_sys_sram_bss_size = _boot_sys_sram_bss_end - _boot_sys_sram_bss_start;

    _sys_sram_raminit_end = .;
    _sys_sram_raminit_size = _sys_sram_raminit_end - _sys_sram_raminit_start;

    _sys_sram_top = SYS_SRAM_END; /* End of internal sram */
    _sys_sram_irq_stack_start = _sys_sram_top - (SYS_SRAM_IRQ_STACKWATCH_SIZE) - (XCPU_IRQ_STACK_SIZE);

    /* Check that the INT SRAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(_sys_sram_irq_stack_start) >= ABSOLUTE(_sys_sram_raminit_end), "INTS", "Internal SRAM overflow !")

    _boot_ram_flash_start_location = .;
    _boot_ram_start = .;
    _boot_ram_flash_end_location = _boot_ram_flash_start_location;

    _boot_ram_bss_start = .;
    _boot_ram_bss_end = .;

    #include "discard.ld"
}

