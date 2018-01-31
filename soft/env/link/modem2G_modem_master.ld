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
/*  - __NATIVE_JAVA__ :  The Native Java need  reload it's data and bss section every time when app start*/
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


#ifdef LOAD_RAMINIT_ELF
ENTRY (boot_LoaderEnter)
#else
ENTRY (boot_Sector)
#endif

/*Mandatory to have the BB side irq handler linked */
EXTERN (__bb_int_handler)
EXTERN (_sxr_Irq_handler)
EXTERN (g_halApMbx)

#ifdef CHIP_HAS_WCPU
EXTERN (_binary_wcpu_main_bin_start)
#endif

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

    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */

    . = EXT_RAM_BASE;

#ifdef SPI_REG_DEBUG
    #include "spi_reg_debug.ld"
#endif

    . = ALIGN(0x100);

    _flash_start = .;

#ifndef LOAD_RAMINIT_ELF
#ifndef RAMRUN
    .boot_code . : AT (ADDR(.boot_code))
    {
        _boot_sector = .;
        *(.boot_sector_start)

        *(.boottext)
        *(.bootrodata)

        *boot_sector.o(ROTEXT() RODATA())
        *boot_sector_chip.o(ROTEXT() RODATA())
        *boot_sys.o(ROTEXT() RODATA())
        *boot_mem_init.o(ROTEXT() RODATA())
        *boot_sector_driver.o(ROTEXT() RODATA())
        *target_boot_config.o(ROTEXT() RODATA())

        . = ALIGN(0x10);
    }
#endif /* !RAMRUN */
#endif /* !LOAD_RAMINIT_ELF */

    /* ============================== */
    /* Main entry point after monitor */
    /* ============================== */

    .main_entry_section . : AT (ADDR(.main_entry_section))
    {
        _code_base = .;
        *boot_loader.o(.text.boot_loader_start)
        *boot_loader.o(ROTEXT() RODATA())
        . = ALIGN(0x10);

        _board_config_start = .;
        *(.board_config)
        . = ALIGN(0x10);
        _board_config_end = .;
    }
    _board_config_size = _board_config_end - _board_config_start;

#ifdef LOAD_BCPU_ELF
    /* ======================================== */
    /* BCPU elf in XCPU elf */
    /* ======================================== */
    #include "link_bcpu_elf.ld"

    . = _bcpu_flash_end;
#endif

    _int_rom_flash_start_location = .;

#ifdef LOAD_RAMINIT_ELF
    /* ======================================== */
    /* raminit image in flash elf */
    /* ======================================== */
    #include "link_raminit_elf.ld"
#else /* !LOAD_RAMINIT_ELF */
#ifdef LOAD_ROM_ELF
    /* ======================================== */
    /* internal rom in flash elf */
    /* ======================================== */
    #include "link_rom_elf.ld"
#endif
#endif /* !LOAD_RAMINIT_ELF */

#if (defined(REWRITE_ROM_AT_BOOT) && defined(FPGA))
    _int_rom_flash_end_location = .;
#else
    _int_rom_flash_end_location = _int_rom_flash_start_location;
#endif

#if (CHIP_ASIC_ID == 10)|| (CHIP_ASIC_ID == 16) /* 8810 */
    /* ================================================= */
    /* in BB SRAM                                        */
    /* ================================================= */
    _bb_sram_flash_start_location = _int_rom_flash_end_location;

    . = BB_SRAM_BASE;
    _bb_sram_start = .;

#if defined(LOAD_ROM_ELF) || defined(LOAD_RAMINIT_ELF)

    _bb_sram_cached_size = 0;
    _bb_sram_uncached_size = 0;

#else /* !LOAD_ROM_ELF && !LOAD_RAMINIT_ELF */

    /*------------------ */
    /* BB SRAM CACHED    */
    /*------------------ */
#ifndef PRELOADED_INT_SRAM
    .bbsram . : AT (_bb_sram_flash_start_location)
#else
    .bbsram . : AT (ADDR(.bbsram))
#endif
    {
        _bb_sram_cached_start = .;
        *(BB_ITLV_SECTION)
        . = ALIGN(0x4);
/*     _bb_sram_itlv_buf_end = .;*/
        *(BB_SPC_CTX_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_cached_end = .;
    _bb_sram_cached_size = _bb_sram_cached_end - _bb_sram_cached_start;

    /*------------------ */
    /* BB SRAM UNCACHED  */
    /*------------------ */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

#ifndef PRELOADED_INT_SRAM
    .bbsramu . : AT (_bb_sram_flash_start_location + _bb_sram_cached_size)
#else
    .bbsramu . : AT (ADDR(.bbsramu))
#endif
    {
        _bb_sram_uncached_start = .;
        . = ALIGN(0x10);
        _mbbsramu = .;
        *(BB_STATIC_SECTION)
        . = ALIGN(0x10);
    }
    _bb_sram_uncached_end = .;
    _bb_sram_uncached_size = _bb_sram_uncached_end - _bb_sram_uncached_start;

#endif /* !LOAD_ROM_ELF && !LOAD_RAMINIT_ELF */

    /* Read the symbol address from BCPU ROM */
    . = _bb_sram_romed_globals_end;

    .bbsram_globals_not_romed . (NOLOAD) : AT (ADDR(.bbsram_globals_not_romed))
    {
        _bb_sram_not_romed_globals_start = .;
        *(BB_SRAM_GLOBALS)
        *(.bbsram_globals)
        _bb_sram_not_romed_globals_end = .;
    }

    LD_ASSERT(ABSOLUTE(BB_SRAM_BASE + BB_SRAM_SIZE - RESERVED_PATCH_SIZE) >= ABSOLUTE(_bb_sram_not_romed_globals_end), "BBGS", "BBSRAM not romed globals overflow out of BBSRAM.")

    _bb_sram_var_size = _bb_sram_cached_size + _bb_sram_uncached_size;
    _bb_sram_var_end = _bb_sram_start + _bb_sram_var_size;

    /*----------------------- */
    /* BB SRAM Patch           */
    /*----------------------- */
#ifndef PRELOADED_INT_SRAM
    _bb_sram_patch_flash_start = _bb_sram_flash_start_location + _bb_sram_var_size;
#else
    _bb_sram_patch_flash_start = _bb_sram_flash_start_location;
#endif

    . = BB_SRAM_BASE + BB_SRAM_SIZE - RESERVED_PATCH_SIZE;
    _bb_sram_patch_start = .;

    .bbsram_patch_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start)
#else
        AT (ADDR(.bbsram_patch_text))
#endif
    {
        _bb_sram_patch_text_start = .;
        *(.bbsram_patch_text)
        *(.bbsram_patch_rodata)
        . = ALIGN(0x10);
        _bb_sram_patch_text_end = .;
    }
    _bb_sram_patch_text_size = _bb_sram_patch_text_end - _bb_sram_patch_text_start;

    .bbsram_patch_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start + _bb_sram_patch_text_size)
#else
        AT (ADDR(.bbsram_patch_data))
#endif
    {
        _bb_sram_patch_data_start = .;
        *(.bbsram_patch_data)
        . = ALIGN(0x10);
        _bb_sram_patch_data_end = .;
    }
    _bb_sram_patch_data_size = _bb_sram_patch_data_end - _bb_sram_patch_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_bb_sram_patch_flash_start + _bb_sram_patch_text_size + _bb_sram_patch_data_size)
#else
        AT (ADDR(.bbsram_patch_ucdata))
#endif
    {
        _bb_sram_patch_ucdata_start = .;
        *(.bbsram_patch_ucdata)
        . = ALIGN(0x10);
        _bb_sram_patch_ucdata_end = .;
    }
    _bb_sram_patch_ucdata_size = _bb_sram_patch_ucdata_end - _bb_sram_patch_ucdata_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _bb_sram_patch_end = .;

    _bb_sram_patch_size = _bb_sram_patch_end - _bb_sram_patch_start;

#ifndef PRELOADED_INT_SRAM
    _bb_sram_patch_flash_end = _bb_sram_patch_flash_start + _bb_sram_patch_size;
#else
    _bb_sram_patch_flash_end = _bb_sram_patch_flash_start;
#endif

    _bb_sram_flash_end_location = _bb_sram_patch_flash_end;

    /*----------------------- */
    /* BB SRAM Patch BSS           */
    /*----------------------- */
    _bb_sram_patch_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_ucbss . (NOLOAD) : AT (ADDR(.bbsram_patch_ucbss))
    {
        *(.bbsram_patch_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bbsram_patch_bss . (NOLOAD) : AT (ADDR(.bbsram_patch_bss))
    {
        *(.bbsram_patch_bss)
        . = ALIGN(0x10);
    }

    _bb_sram_patch_bss_end = .;
    _bcpu_dualport_bss_size = _bb_sram_patch_bss_end - _bb_sram_patch_bss_start;

    LD_ASSERT(ABSOLUTE(BB_SRAM_BASE + BB_SRAM_SIZE) >= ABSOLUTE(_bb_sram_patch_bss_end),
                        "BBPC", "BBSRAM patch code overflow out of BBSRAM.")
#endif /* 8810 */

    /* ===================================================== */
    /* in BB DUALPORT SRAM                                      */
    /* ===================================================== */
#if (CHIP_ASIC_ID == 10)|| (CHIP_ASIC_ID == 16) /* 8810 */
    _dualport_sram_flash_start_location = _bb_sram_flash_end_location;
    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location;
#else /* 8850 or later */
    _dualport_sram_flash_start_location = _int_rom_flash_end_location;
    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location;
#endif

    /* ===================================================== */
    /* in AP DUALPORT SRAM                                      */
    /* ===================================================== */
    . = AP_DUALPORT_SRAM_BASE;

    _ap_dualport_sram_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .ap_mbx . (NOLOAD) : AT(ADDR(.ap_mbx))
    {
        KEEP(*hal_ap_mailbox.o(.ap_mbx))
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _ap_dualport_sram_end = .;

    LD_ASSERT(ABSOLUTE(AP_DUALPORT_SRAM_BASE + AP_DUALPORT_SRAM_SIZE) >= ABSOLUTE(_ap_dualport_sram_end), "DPOV", "AP DUALPORT SRAM overflow.")

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */

    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */

#ifndef LOAD_RAMINIT_ELF
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

    . = SYS_SRAM_AVAILABLE_BASE;

    /* ------------------------------------------------ */
    /* SYS SRAM IRQ - IRQ HANDLER IN SYS SRAM */
    /* ------------------------------------------------ */
    _irq_sys_sram_flash_start_location = _dualport_sram_flash_end_location;

    _irq_sys_sram_start = .;
#ifndef PRELOADED_INT_SRAM
    .irqsram . : AT (_irq_sys_sram_flash_start_location)
#else
    .irqsram . : AT (ADDR(.irqsram))
#endif
    {
        *hal_irq_handler.o(.irqtext)
        *hal_irq_handler.o(.text .text.* .sramtext .rdata .rodata .rodata.*)
        . = ALIGN(0x10);
    }
    _irq_sys_sram_end = .;
    _irq_sys_sram_size = _irq_sys_sram_end - _irq_sys_sram_start;

#ifndef PRELOADED_INT_SRAM
    _irq_sys_sram_flash_end_location = _irq_sys_sram_flash_start_location + _irq_sys_sram_size;
#else
    _irq_sys_sram_flash_end_location = _irq_sys_sram_flash_start_location;
#endif

#ifdef LOAD_BCPU_ELF
    . = _bcpu_sys_sram_end;
#endif

#ifdef LOAD_RAMINIT_ELF

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;

#else /* !LOAD_RAMINIT_ELF */

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;

    _boot_sys_sram_start = .;

    _boot_sys_sram_txt_start = .;
#ifndef RAMRUN
    .bootsramtext . : AT (_boot_sys_sram_flash_start_location)
#else
    .bootsramtext . : AT (ADDR(.bootsramtext))
#endif
    {
#ifdef RAMRUN
        _boot_sector = .;
        *(.boot_sector_start)

        *(.boottext)
        *(.bootrodata)

        *boot_sector.o(ROTEXT() RODATA())
        *boot_sector_chip.o(ROTEXT() RODATA())
        *boot_sys.o(ROTEXT() RODATA())
        *boot_mem_init.o(ROTEXT() RODATA())
        *boot_sector_driver.o(ROTEXT() RODATA())
        *target_boot_config.o(ROTEXT() RODATA())
#endif /* RAMRUN */

        *(.bootsramtext)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_txt_end = .;
    _boot_sys_sram_txt_size = _boot_sys_sram_txt_end - _boot_sys_sram_txt_start;

#ifndef RAMRUN
    _boot_sys_sram_data_load_start = _boot_sys_sram_flash_start_location + _boot_sys_sram_txt_size;
#else
    _boot_sys_sram_data_load_start = .;

#ifdef SYS_SRAM_DATA_SIZE
    . += SYS_SRAM_DATA_SIZE;
#endif
#endif

    _boot_sys_sram_data_start = .;
    .bootsramdata . : AT (_boot_sys_sram_data_load_start)
    {
        *(.bootsramdata)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_data_end = .;
    _boot_sys_sram_data_size = _boot_sys_sram_data_end - _boot_sys_sram_data_start;

    _boot_sys_sram_end = .;
    _boot_sys_sram_size = _boot_sys_sram_end - _boot_sys_sram_start;

#ifndef RAMRUN
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start + _boot_sys_sram_data_size;

    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location + _boot_sys_sram_size;

#else /* RAMRUN */

#if defined(SYS_SRAM_DATA_SIZE) && (SYS_SRAM_DATA_SIZE > 0)
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start + _boot_sys_sram_data_size;

    /* Check that the boot sys sram data is not over overflowed */
    LD_ASSERT(ABSOLUTE(_boot_sys_sram_data_start) >= ABSOLUTE(_boot_sys_sram_data_load_end), "BDOV", "Boot sys sram data overflow (SYS_SRAM_DATA_SIZE too small) !")
#else
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start;
#endif

    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location;
#endif /* RAMRUN */

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _boot_sys_sram_flash_end_location;

#endif /* !LOAD_RAMINIT_ELF */

    _sys_sram_start = .;
    _sys_sram_cached_start = .;

#ifndef PRELOADED_INT_SRAM
    .sramtext . : AT (_sys_sram_flash_start_location)
#else
    .sramtext . : AT (ADDR(.sramtext))
#endif
    {
        _sys_sram_txt_start = .;

        /* Some .bootsramtext functions are not included in RAMINIT elf */
        *(.bootsramtext)
        *(.sramtext .sramrodata)

        *pal_gsmr.o(ROTEXT())
        *sxr_mem.o(ROTEXT())
        *sxr_jbsd.o(ROTEXT())

#if (CHIP_ASIC_ID != 8) /* not 8808 */
        *wifi_sha1.o(ROTEXT() RODATA())
#endif

        . = ALIGN(0x10);
    }
    _sys_sram_txt_end = .;
    _sys_sram_txt_size = _sys_sram_txt_end - _sys_sram_start;


#ifndef PRELOADED_INT_SRAM
    .sramdata _sys_sram_txt_end : AT (_sys_sram_flash_start_location + _sys_sram_txt_size )
#else
    .sramdata _sys_sram_txt_end : AT (ADDR(.sramdata))
#endif
    {
        _sys_sram_data_start = .;

        /* Some .bootsramdata variables are not included in RAMINIT elf */
        *(.bootsramdata)
        *(.sramdata)

        *pal_gsmr.o(.data .data.* .gnu.linkonce.d.* .data1)
        *pal_gsmr.o(.sdata .sdata.* .gnu.linkonce.s.*)
        *pal_gsmr.o(.sdata2 .sdata2.* .gnu.linkonce.s2.*)

        *(.srrodata)
        . = ALIGN(0x10);

    }
    _sys_sram_data_end = .;
    _sys_sram_cached_end = .;
    _sys_sram_data_size = _sys_sram_data_end - _sys_sram_data_start;
    _sys_sram_cached_size = _sys_sram_cached_end - _sys_sram_cached_start;

    /*-------------------- */
    /* SYS SRAM UNCACHED   */
    /*-------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

#ifndef PRELOADED_INT_SRAM
    .sramucdata . : AT (_sys_sram_flash_start_location + _sys_sram_cached_size)
#else
    .sramucdata . : AT (ADDR(.sramucdata))
#endif
    {
        _sys_sram_uncached_start = .;
        *(.sramucdata)
        *(.srroucdata)
        . = ALIGN(0x10);
    }
    _sys_sram_uncached_end = .;
    _sys_sram_uncached_size = _sys_sram_uncached_end - _sys_sram_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _sys_sram_end =  .;
    _sys_sram_size = _sys_sram_end - _sys_sram_start;

#ifndef PRELOADED_INT_SRAM
    _sys_sram_flash_end_location = _sys_sram_flash_start_location + _sys_sram_size;
#else
    _sys_sram_flash_end_location = _sys_sram_flash_start_location;
#endif

#ifndef LOAD_RAMINIT_ELF
    /* ------------------------------------------------ */
    /* BOOTING BSS IN SYS SRAM (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_sys_sram_bss_start = .;
    .bootsrambss . (NOLOAD) : AT (ADDR(.bootsrambss))
    {
        *(.bootsrambss)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_bss_end = .;
    _boot_sys_sram_bss_size = _boot_sys_sram_bss_end - _boot_sys_sram_bss_start;
#endif /* !LOAD_RAMINIT_ELF */

    /*----------------------- */
    /* SYS SRAM BSS           */
    /*----------------------- */
    _sys_sram_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .sramucbss . (NOLOAD) : AT (ADDR(.sramucbss))
    {
        *(.sramucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .srambss . (NOLOAD) : AT (ADDR(.srambss))
    {
        *(.srambss)
        . = ALIGN(0x10);

    }

    _sys_sram_bss_end = .;
    _sys_sram_bss_size = _sys_sram_bss_end - _sys_sram_bss_start;

    #include "int_sram_plus.ld"
    _sys_sram_overlay_flash_start_location = _sys_sram_plus_flash_end_location;

    /* Overlay in SYS SRAM*/
    _sys_sram_overlay_start = .;
    OVERLAY . : NOCROSSREFS AT (_sys_sram_overlay_flash_start_location)
    {
        #include "int_sram_overlay.ld"
    }

    /* ld-2.15 End of overlay bug workaround (Manual calculation)*/
    _sys_sram_overlay_end           = _sys_sram_overlay_start + MAX_SIZE_OF_SYS_SRAM_OVERLAY;
    . = _sys_sram_overlay_end;

    _sys_sram_overlay_flash_size    = FLASH_SIZE_OF_SYS_SRAM_OVERLAY;

    _sys_sram_overlay_flash_end_location = _sys_sram_overlay_flash_start_location + _sys_sram_overlay_flash_size;

    /*----------------------- */
    /* SYS SRAM HEAP 1        */
    /*----------------------- */
    _sys_sram_heap_start = _sys_sram_overlay_end;

#ifdef LOAD_RAMINIT_ELF
    _sys_sram_heap_end = _sys_sram_raminit_start;
#else
    _sys_sram_top = SYS_SRAM_END; /* End of internal sram */
    _sys_sram_heap_end = _sys_sram_top - (SYS_SRAM_IRQ_STACKWATCH_SIZE) - (XCPU_IRQ_STACK_SIZE);
#endif

    _sys_sram_heap_size = _sys_sram_heap_end - _sys_sram_heap_start;

    /* Check that the INT SRAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(_sys_sram_heap_end) > ABSOLUTE(_sys_sram_heap_start), "INTS", "Internal SRAM overflow !")

    /*----------------------- */
    /* SYS SRAM IRQ STACK WATCH */
    /*----------------------- */
    _sys_sram_irq_stackwatch_size = (SYS_SRAM_IRQ_STACKWATCH_SIZE) ;
    _sys_sram_irq_stackwatch_start = _sys_sram_heap_end;
    _sys_sram_irq_stackwatch_end = _sys_sram_irq_stackwatch_start + _sys_sram_irq_stackwatch_size;

    /* Caution, _sys_sram_non_romed_top is used by the boot ROM code. */
    .sram_romed_top (SYS_SRAM_END) (NOLOAD) : AT (ADDR(.sram_romed_top))
    {
        _sys_sram_non_romed_top = .;
    }

    /*---------------- */
    /* End of SYS SRAM */
    /*---------------- */

#if (CHIP_HAS_PROXY_MEMORY == 1)
    /* ========================================== */
    /* Proxy Memory                               */
    /* ========================================== */
    /* Standard usage of proxy memory */
    /* Manual placing and copy from flash */
    _proxy_memory_flash_start_location = _sys_sram_overlay_flash_end_location;
#ifndef PRELOADED_PROXY_MEMORY
    .proxy_memory SYS_PROXY_MEM_BASE : AT (_proxy_memory_flash_start_location)
#else
    .proxy_memory SYS_PROXY_MEM_BASE : AT (ADDR(.proxy_memory))
#endif
    {
        _proxy_memory_start = .;
        /* Loadable as a ramrun since the proxy memory is enabled
         * by the romed monitor. */
        *sxs_rlm.o(.text .text.*)
        *sxr_rlm.o(.text .text.*)
        *hal_timer.o(.text .text.*)
        *sxr_tksd.o(.text .text.*)
        *sxs_io.o(.text .text.*)
        *sxr_tls.o(.text .text.*)
        *sxs_tls.o(.text .text.*)
        *sxr_sbx.o(.text .text.*)
        *sxs_sbx.o(.text .text.*)
        *sxr_tim.o(.text .text.*)
        *hal_trace.o(.text .text.*)
        . = ALIGN(0x10);
    }

    _proxy_memory_end = . ;
    _proxy_memory_size = _proxy_memory_end - _proxy_memory_start ;

#ifndef PRELOADED_PROXY_MEMORY
    _proxy_memory_flash_end_location = _proxy_memory_flash_start_location + _proxy_memory_size;
#else
    _proxy_memory_flash_end_location = _proxy_memory_flash_start_location;
#endif

    /* Overlay in Proxy Memory */
    _fast_cpu_memory_overlay_start = .;
    OVERLAY . : NOCROSSREFS AT (_proxy_memory_flash_end_location)
    {
        #include "fast_cpu_memory_overlay.ld"
    }

    /* ld-2.15 End of overlay bug workaround (Manual calculation)*/
    _fast_cpu_memory_overlay_end           = _fast_cpu_memory_overlay_start + MAX_SIZE_OF_FAST_CPU_MEMORY_OVERLAY;

    . = _fast_cpu_memory_overlay_end;

    _fast_cpu_memory_overlay_flash_size    = FLASH_SIZE_OF_FAST_CPU_MEMORY_OVERLAY;
    _fast_cpu_memory_overlay_flash_end_location = _proxy_memory_flash_end_location + _fast_cpu_memory_overlay_flash_size;

    _proxy_memory_top = SYS_PROXY_MEM_BASE + SYS_PROXY_MEM_SIZE - 4;
    _proxy_memory_overlay_top = _proxy_memory_top - XCPU_IRQ_STACK_SIZE;
    LD_ASSERT(ABSOLUTE(_fast_cpu_memory_overlay_end) < ABSOLUTE(_proxy_memory_overlay_top), "SPMO", "System Proxy Memory Overlfow !!!")

    /* ========================================== */
    /* End of Proxy Memory                        */
    /* ========================================== */
#else /* CHIP_HAS_PROXY_MEMORY */
    _fast_cpu_memory_overlay_flash_end_location = _sys_sram_overlay_flash_end_location;
#endif /* CHIP_HAS_PROXY_MEMORY */


    /* ========================================== */
    /* continued in External RAM                  */
    /* ========================================== */

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _ram_flash_start_location = _fast_cpu_memory_overlay_flash_end_location;

    . = _ram_flash_start_location;

    _ram_start = .;

    /* ------------------------------------------------ */
    /* Ext RAM CACHED  - SECTIONS ALWAYS IN RAM */
    /* ------------------------------------------------ */
    .ram . : AT (ADDR(.ram))
    {
        _ram_special_cached_start = .;
        _ram_txt_start = .;
        *(.ram)
        . = ALIGN(0x10);
    }
    _ram_special_cached_end = .;
    _ram_special_cached_size = _ram_special_cached_end - _ram_special_cached_start;

    /*---------------------------- */
    /* Ext RAM CACHED TXT          */
    /*---------------------------- */
    .ramtext _ram_special_cached_end : AT (ADDR(.ramtext))
    {
        _ram_cached_txt_start = .;

        /* Some .bootramtext functions are not included in RAMINIT elf */
        *(.bootramtext)

#ifndef LOAD_RAMINIT_ELF
        *drv_ram.o(ROTEXT() RODATA())
#endif

#ifdef CODE_IN_RAM
        /* Some .boottext functions are not included in RAMINIT elf */
        *(.boottext)
        *(.text .text.* .stub .gnu.linkonce.t.*)
#endif

        /* libgcc's softfloat library */
        _pack*f.o(.text .text.* .rodata .rodata.*)
        _unpack*f.o(.text .text.* .rodata .rodata.*)
        _*f.o(.text .text.* .rodata .rodata.*)
        *_lshrdi3.o(.text .text.* .rodata .rodata.*)
        *_ashldi3.o(.text .text.* .rodata .rodata.*)
        *_fixunssfsi.o(.text .text.* .rodata .rodata.*)
        *_fixunsdfsi.o(.text .text.* .rodata .rodata.*)
        *_fixdfdi.o(.text .text.* .rodata .rodata.*)
        *_fixunsdfdi.o(.text .text.* .rodata .rodata.*)
        *_divdi3.o(.text .text.* .rodata .rodata.*)
        *_moddi3.o(.text .text.* .rodata .rodata.*)
        *_udivdi3.o(.text .text.* .rodata .rodata.*)
        *_umoddi3.o(.text .text.* .rodata .rodata.*)
        *_sf_to_si.o(.text .text.* .rodata .rodata.*)
        *_df_to_si.o(.text .text.* .rodata .rodata.*)
        *_divdi3.o(.text .text.* .rodata .rodata.*)
        *_ashrdi3.o(.text .text.* .rodata .rodata.*)
        *_clzsi2.o(.text .text.* .rodata .rodata.*)

#ifdef CONST_IN_RAM
        /* Some .bootrodata variables are not included in RAMINIT elf */
        *(.bootrodata)
        *(.rdata)
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.rodata1)
        *(.ctors)
#endif

        . = ALIGN(0x10);
    } = 0
    _ram_cached_txt_end = .;
    ram_cached_txt_size = _ram_cached_txt_end - _ram_cached_txt_start;

    _ram_txt_end = .;

    _ram_end = .;
    _ram_size = _ram_end - _ram_start;

    _ram_flash_end_location = _ram_flash_start_location;


    /*========================================================= */
    /* In FLASH                                                 */
    /*========================================================= */
    _rom_flash_start_location = .;

    .rotext . : AT (ADDR(.rotext))
    {
#ifndef CODE_IN_RAM
        /* Some .boottext functions are not included in RAMINIT elf */
        *(.boottext)
        *(.text)
        *(.text.*)
        *(.stub .gnu.linkonce.t.*)
#endif
        . = ALIGN(0x20);
    }

    .rodata . : AT (ADDR(.rodata))
    {
#ifndef CONST_IN_RAM
        /* Some .bootrodata variables are not included in RAMINIT elf */
        *(.bootrodata)
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        *(.rodata.*_png)
        *(.gnu.linkonce.r.*)
        *(.rodata1)
        *(.roresdata)
        *(.robsdata)
#if (EXTRA_CODE_SIZE == 0)
        *(.extra)
#endif
#endif
        . = ALIGN(0x20);
    }


    /*=================================================== */
    /* Placeholder for misc hacks, not used for real apps */
    /*=================================================== */
    .misc . : AT (ADDR(.misc))
    {
        *(.misc)
         . = ALIGN(0x10);
    }

    _flash_end = .;

    /* kept for compatibility */
    _etext = _ram_txt_end;
    _esramtext = _sys_sram_txt_end;
    _end = _ram_bss_end;


    /*=================================================== */
    /* Codes to fill the hole between user sector and     */
    /* calib sector                                       */
    /*=================================================== */
#if (EXTRA_CODE_SIZE != 0)
    .extra EXTRA_CODE_BASE : AT(ADDR(.extra))
    {
        _extra_section_start = .;
        *(.extra)
        . = ALIGN(0x10);
    }
    _extra_section_end = .;
    _extra_section_size = _extra_section_end - _extra_section_start;
    extra_rores_size = EXTRA_CODE_SIZE;

    LD_ASSERT(ABSOLUTE(extra_rores_size) > ABSOLUTE(_extra_section_size), "FLAS", "Extra section overflow: Please check EXTRA_RORES_SIZE in memd.def and the .extra section size.")
    extra_remain_size = extra_rores_size - _extra_section_size;
#endif

    _boot_ram_data_load_start = .;

#ifdef EXT_RAM_DATA_SIZE
    . += EXT_RAM_DATA_SIZE;
#endif

#ifdef LOAD_RAMINIT_ELF
    _boot_ram_size = 0;
#else
    /* ------------------------------------------------ */
    /* BOOTING DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _boot_ram_start = .;

    _boot_ram_cached_data_start = .;
    .bootdata . : AT (_boot_ram_data_load_start)
    {
        *(.bootdata)

        *drv_ram.o(DATA())
        . = ALIGN(0x10);
    }
    _boot_ram_cached_data_end = .;
    _boot_ram_cached_data_size = _boot_ram_cached_data_end - _boot_ram_cached_data_start;

    _boot_ram_end = .;
    _boot_ram_size = _boot_ram_end - _boot_ram_start;

    _boot_ram_flash_start_location = _boot_ram_data_load_start;
    _boot_ram_flash_end_location = _boot_ram_flash_start_location + _boot_ram_size;
#endif /* !LOAD_RAMINIT_ELF */

    _ram_data_load_start = _boot_ram_data_load_start + _boot_ram_size;
    _ram_data_start = .;

    /*---------------------- */
    /* Ext RAM UNCACHED Data */
    /*---------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .ucdata . : AT (_ram_data_load_start)
    {
        _ram_uncached_data_start = .;
        *(.ucdata)
        *(*.globalvars.uc)
        . = ALIGN(0x10);
    }
    _ram_uncached_data_end = .;
    _ram_uncached_data_size = _ram_uncached_data_end - _ram_uncached_data_start;

    /*---------------------- */
    /* Ext RAM CACHED Data */
    /*---------------------- */
    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .data . : AT (_ram_data_load_start + _ram_uncached_data_size)
    {
        _ram_cached_data_start = . ;

        /* Some .bootdata variables are not included in RAMINIT elf */
        *(.bootdata)

        /* NOTE: globalvars.c sections are to be removed.
         * They are kept here for backward compatibility of SVC/VPP codes.
         * These codes are linked from Total_BJ branch and the special global
         * variable sections will be removed in the future. */
        *(*.globalvars.c *.globalvars.c.*)

        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d.*)
        *(.data1)
        _gp = ALIGN(16) + 0x7ff0;
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.lit8)
        *(.lit4)
        . = ALIGN(0x4);
        /***barrier : do not remove or edit */
        . = ALIGN(0x10);
    }
    _ram_cached_data_end = .;
    _ram_cached_data_size = _ram_cached_data_end - _ram_cached_data_start;

    _ram_data_end = .;
    _ram_data_size = _ram_data_end - _ram_data_start;
#ifndef RAMRUN
    _ram_data_load_end = _ram_data_load_start + _ram_data_size;
#else
#if defined(EXT_RAM_DATA_SIZE) && (EXT_RAM_DATA_SIZE > 0)
    /* Check that the ram data is not over overflowed */
    LD_ASSERT(ABSOLUTE(_ram_data_start) >= ABSOLUTE(_ram_data_load_end), "DAOV", "Ram data overflow (EXT_RAM_DATA_SIZE too small) !")
    _ram_data_load_end = _ram_data_load_start + _ram_data_size;
#else
    _ram_data_load_end = _ram_data_load_start;
#endif

#endif
    /*-------------------------------------------------------  */
    /* Uninitialized section in RAM (content kept after reboot)*/
    /*-------------------------------------------------------  */
    .backup . (NOLOAD) : AT (ADDR(.backup))
    {
        *(.backup)
        . = ALIGN(0x10);
    }

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .ucbackup . (NOLOAD) : AT (ADDR(.ucbackup))
    {
        *(.ucbackup)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

#ifndef LOAD_RAMINIT_ELF
    /* ------------------------------------------------ */
    /* BOOTING BSS (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_ram_bss_start = .;
    .bootbss . (NOLOAD) : AT (ADDR(.bootbss))
    {
        *(.bootbss)
        . = ALIGN(0x10);
    }
    _boot_ram_bss_end = .;
    _boot_ram_bss_size = _boot_ram_bss_end - _boot_ram_bss_start;
#endif /* !LOAD_RAMINIT_ELF */

    /*-------------------------------------------------------  */
    /* RAM BSS (content reset to 0 )                           */
    /*-------------------------------------------------------  */
    _ram_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .ucbss . (NOLOAD) : AT (ADDR(.ucbss))
    {
        *(.ucbss)
        *(.ramucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bss . (NOLOAD) : AT (ADDR(.bss))
    {
        /* Some .bootbss variables are not included in RAMINIT elf */
        *(.bootbss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.rambss)
        . = ALIGN(0x10);
    }

    _ram_bss_end = .;
    _ram_bss_size = _ram_bss_end - _ram_bss_start;

    /*----------- */
    /* End of RAM */
    /*----------- */
    /* _ram_top = EXT_RAM_BASE + RAM_PHY_SIZE - (calib sector size + factory sector size)
     *          = EXT_RAM_BASE + RAM_SIZE
     *          = CALIB_BASE
     */
    _ram_top = CALIB_BASE;
    _heap_size = _ram_top -_ram_bss_end;
    _heap_start =  _ram_bss_end;
    _heap_end =  _ram_bss_end + _heap_size;

    /* Check that the external RAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(_ram_top) > ABSOLUTE(_ram_bss_end), "EXTS", "External RAM overflow !")


    /* ============================== */
    /* calib sector in flash          */
    /* ============================== */

    .calib CALIB_BASE : AT (ADDR(.calib))
    {
        _calib_start = .;
        *(.calib);
        . = ALIGN(0x10);
    }
    _calib_end = .;
    _calib_size = _calib_end - _calib_start;


    /* ============================== */
    /* factory settings in flash      */
    /* ============================== */

    .factory_settings FACT_SETTINGS_BASE : AT (ADDR(.factory_settings))
    {
        _factory_start = .;
        *(.factory_settings)
        . = ALIGN(0x10);
    }
    _factory_end = .;
    _factory_size = _factory_end - _factory_start;

    /* Legal layout patterns:
     * Code (including boot code) -> Calib Data -> Factory Data
     * /

    /*Check that ram code and data do not overwrite
     * the calib data or factory setting space */
    LD_ASSERT(ABSOLUTE(_calib_start) >= ABSOLUTE(_ram_top),
              "CFOV", "Calib data is overlapped with ram code and data.")

    /*Check that the calib data in flash do not overwrite
     * the factory setting data space */
    LD_ASSERT(ABSOLUTE(_factory_start) > ABSOLUTE(_calib_start),
              "FACO", "Factory setting data is overlapped with calib data.")


    /* ============================== */
    /* WCPU sections                  */
    /* ============================== */

#if CHIP_HAS_WCPU  

    . = WD_RAM_BASE;

    .wdrodata . : AT (ADDR(.wdrodata))
    {
        _wcpu_main = .;
        #if !defined (MODEM_RECOVER_IMG)
       /* KEEP(*wcpu_main.bin.o(.wdrodata))*/
        #endif
        . = ALIGN(0x10);
    }

    . = WD_DUALPORT_SRAM_BASE; 

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .2g3ginmailbox . : AT (ADDR(.2g3ginmailbox))
    {
        _2g3ginmailbox_START = .;
        *(.2g3ginmailbox*) 
     }

    .2g3goutmailbox . : AT (ADDR(.2g3goutmailbox))
    {
        _2g3goutmailbox_START = .;
        *(.2g3goutmailbox*) 
     }

    .wd_mailbox_Trace . : AT (ADDR(.wd_mailbox_Trace))
    {
        _wd_mailbox_Trace_START = .;
        *(.wd_mailbox_Trace*) 
     }

    . = WD_DUALPORT_SRAM_BASE + WD_DUALPORT_SRAM_SIZE; 

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .wd_mailbox . : AT (ADDR(.wd_mailbox))
    {
        _wd_mailbox_START = .;
        *(.wd_mailbox*) 
     }

#endif /* CHIP_HAS_WCPU */

 #if (CHIP_HAS_TMCU==1)
 	. = TMCU_DM_SRAM_BASE;
	tmcu_dm_start_location = .;
	. = TMCU_DM_SRAM_BASE + TMCU_DM_SRAM_SIZE;
	tmcu_dm_end_location = .;
	. = TMCU_PM_SRAM_BASE;
	tmcu_pm_start_location = .;
	. = TMCU_PM_SRAM_BASE + TMCU_PM_SRAM_SIZE;
	tmcu_pm_end_location = .;
	
	
	. = TMCU_SHARE_BBSRAM1_BASE;
	tmcu_share_bbsram1_start_location = .;
	. = TMCU_SHARE_BBSRAM1_BASE + TMCU_SHARE_BBSRAM1_SIZE;
	tmcu_share_bbsram1_end_location = .;
	
	. = TMCU_SHARE_BBSRAM2_BASE	;
	tmcu_share_bbsram2_start_location = .;
	. = TMCU_SHARE_BBSRAM2_BASE + TMCU_SHARE_BBSRAM2_SIZE;
	tmcu_share_bbsram2_end_location = .;
	
	. = TMCU_SHARE_PMSRAM1_BASE;
	tmcu_share_pmsram1_start_location = .;
	. = TMCU_SHARE_PMSRAM1_BASE + TMCU_SHARE_PMSRAM1_SIZE;
	tmcu_share_bbsram1_end_location = .;
	
	. = TMCU_SHARE_PMSRAM2_BASE	;
	tmcu_share_pmsram2_start_location = .;
	. = TMCU_SHARE_PMSRAM2_BASE + TMCU_SHARE_PMSRAM2_SIZE;
	tmcu_share_pmsram2_end_location = .;

 #endif
 		
    #include "discard.ld"
}

