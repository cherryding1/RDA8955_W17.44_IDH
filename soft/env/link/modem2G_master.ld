/* ======================================================================= */
/*                  TARGET CONFIGURATION SECTION                           */
/* ======================================================================= */
/* modem2g_master.ld */
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
/*Mandatory to have the BB side irq handler linked */
EXTERN (__bb_int_handler)
EXTERN (_sxr_Irq_handler)
EXTERN (boot_LoaderEnter)
EXTERN (boot_Sector)
EXTERN (boot_Sector_Nand)


SECTIONS
{
    .check_boot_banned_sections :
    {
        FILENAME_PREFIX(boot_*.o, BOOT_BANNED_SECTIONS())
        FILENAME_PREFIX(target_boot_config.o, BOOT_BANNED_SECTIONS())
        FILENAME_PREFIX(drv_flsh_map.o, BOOT_BANNED_SECTIONS())
        /* For backward compatibility, some object files are allowed to
         * have normal data or bss sections, e.g., drv_flsh.o
         */
    }
    LD_ASSERT(SIZEOF(.check_boot_banned_sections) == 0, "BSER", "Boot files cannot contain normal data or bss sections")

#ifdef COMPRESS_PROCESS_ENABLE
    /* The load address of the compressed code should be set */
    /* to an address at which no LOD data will be generated. */
    /* Here it is set to (EXT_RAM_BASE + RAM_SIZE). */
    _compressed_code_load_start = EXT_RAM_BASE + RAM_SIZE;
#endif

#if !(defined(COMPRESS_PROCESS_ENABLE) && defined(LINK_STAGE_2))

    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */

    . = EXT_RAM_BASE;

#ifdef SPI_REG_DEBUG
    #include "spi_reg_debug.ld"
#endif

    . = ALIGN(0x100);

    /* In RAMRUN, all the RAM used by BCPU will be indicated by _bcpu_flash_end
     * instead of _bcpu_ram_bss_end */
#ifdef LOAD_BCPU_ELF
#ifndef RAMRUN
    . = _bcpu_ram_bss_end;
#endif
#endif

    . = ALIGN(0x100);

    _ram_available_base = .;

#ifndef RAMRUN
    /* ========================================== */
    /* set current location counter to flash base */
    /* ========================================== */

#if (NAND_FLASH == 1)
    /*----------------------- */
    /* NAND FLASH BOOT CODE   */
    /*----------------------- */
    /* TODO: This reserved nand boot code space can be reused after jumping to SDRAM */
    .dualport_nand_flash_boot (BB_DUALPORT_SRAM_BASE) : AT (FLASH_BASE)
    {
        _nand_flash_boot_start = .;
#if defined(ROMULATOR)
        LONG(0xD9EF0045)
#else
        LONG(0xFFFFFFFF)
#endif /* ROMULATOR */

        . = ALIGN(0x10);
        _boot_sector_nand = .;
        *boot_sector_nand.o(.boot_sector_nand_start)
        *boot_sector_nand.o(ROTEXT() RODATA())
        . = ALIGN(0x10);
    }
    _nand_flash_boot_end = .;

    /* Check that the NAND flash booting code is not over stuffed */
    LD_ASSERT(ABSOLUTE(_nand_flash_boot_start) + NAND_FLASH_BOOT_CODE_SIZE >=
              ABSOLUTE(_nand_flash_boot_end), "NAND", "NAND flash booting code overflow !")

    . = FLASH_BASE + NAND_FLASH_BOOT_CODE_SIZE;

#else /* NAND_FLASH != 1 */

    . = FLASH_BASE + FLASH_BOOT_OFFSET;

#endif /* NAND_FLASH != 1 */

#endif /* !RAMRUN */

    _flash_start = .;
    /* ============================= */
    /* boot sector in flash          */
    /* ============================= */
    .boot_code _flash_start : AT (ADDR(.boot_code))
    {
/* keep 4 word boot sector offset */
#if defined(ROMULATOR)
        LONG(0xD9EF0045)
#else
        LONG(0xFFFFFFFF)
#endif /* ROMULATOR */

#ifndef RAMRUN
        . = ALIGN(0x10);
        _boot_sector = .;
        *(.boot_sector_start)

        *(.boottext)
        *(.bootrodata)

        *boot_sector.o(ROTEXT() RODATA())
        *boot_sector_chip.o(ROTEXT() RODATA())
        *boot_sys.o(ROTEXT() RODATA())
        *boot_mem_init.o(ROTEXT() RODATA())
        *boot_key.o(ROTEXT() RODATA())
        *boot_sector_driver.o(ROTEXT() RODATA())
        *target_boot_config.o(ROTEXT() RODATA())
#endif /* RAMRUN */

        . = ALIGN(0x10);
    }

#ifdef LOAD_BCPU_ELF
    /* ======================================== */
    /* BCPU elf in XCPU elf */
    /* ======================================== */
    #include "link_bcpu_elf.ld"

    . = _bcpu_flash_end;
#endif

    /* ============================== */
    /* Main entry point after monitor */
    /* ============================== */

#ifndef RAMRUN
#if (CODE_BASE != FLASH_BASE)
    /* ========================================== */
    /* set current location counter to code base if it differs from flash base */
    /* ========================================== */
    LD_ASSERT(ABSOLUTE(CODE_BASE) >= ABSOLUTE(.), "CBE", "CODE_BASE is invalid. \
Note that CODE_BASE is designed to be used only when calib and factory sectors are located at flash forepart, \
and it should never be set in all other cases.")

    . = CODE_BASE;

#endif /* CODE_BASE != FLASH_BASE */
#endif /* RAMRUN */

    .main_entry_section . : AT (ADDR(.main_entry_section))
    {
#ifndef RAMRUN
        _code_base = .;
        *boot_loader.o(.text.boot_loader_start)
        *boot_loader.o(ROTEXT() RODATA())
        . = ALIGN(0x10);
#endif /* RAMRUN */

        _board_config_start = .;
        *(.board_config)
        . = ALIGN(0x10);
        _board_config_end = .;
    }
    _board_config_size = _board_config_end - _board_config_start;


    _int_rom_flash_start_location = .;

#ifdef LOAD_ROM_ELF
    /* ======================================== */
    /* internal rom in flash elf */
    /* ======================================== */
    #include "link_rom_elf.ld"
#endif

#if (defined(REWRITE_ROM_AT_BOOT) && defined(FPGA))
    _int_rom_flash_end_location = .;
#else
    _int_rom_flash_end_location = _int_rom_flash_start_location;
#endif


#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) /* GALLITE or 8808 */
    /* ================================================= */
    /* in BB SRAM                                        */
    /* ================================================= */
    _bb_sram_flash_start_location = _int_rom_flash_end_location;

    . = BB_SRAM_BASE;
    _bb_sram_start = .;

#ifdef LOAD_ROM_ELF

    _bb_sram_cached_size = 0;
    _bb_sram_uncached_size = 0;

#else /* !LOAD_ROM_ELF */

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

#endif /* !LOAD_ROM_ELF */

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

#endif /* GALLITE or 8808 */

    /* ===================================================== */
    /* in BB DUALPORT SRAM                                      */
    /* ===================================================== */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) /* GALLITE or 8808 */
    _dualport_sram_flash_start_location = _bb_sram_flash_end_location;
#else
    _dualport_sram_flash_start_location = _int_rom_flash_end_location;
#endif

#if (CHIP_ASIC_ID == 6) /* GALLITE */

    _dualport_sram_size = 0;

#else /* 8808 or later */

#if (NAND_FLASH == 1)
    . = BB_DUALPORT_SRAM_BASE + NAND_FLASH_BOOT_CODE_SIZE;
#else
    . = BB_DUALPORT_SRAM_BASE;
#endif

#ifdef LOAD_BCPU_ELF
    . = _bcpu_dualport_sram_end;
#endif

    _dualport_sram_start = .;

    .dualport_sram_text . :
#ifndef PRELOADED_INT_SRAM
        AT (_dualport_sram_flash_start_location)
#else
        AT (ADDR(.dualport_sram_text))
#endif
    {
        _dualport_sram_text_start = .;
        *(.dualport_sram_text)
        *(.dualport_sram_rodata)
        . = ALIGN(0x10);
        _dualport_sram_text_end = .;
    }
    _dualport_sram_text_size = _dualport_sram_text_end - _dualport_sram_text_start;

    .dualport_sram_data . :
#ifndef PRELOADED_INT_SRAM
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size)
#else
        AT (ADDR(.dualport_sram_data))
#endif
    {
        _dualport_sram_cached_data_start = .;
        *(.dualport_sram_data)
        . = ALIGN(0x10);
        _dualport_sram_cached_data_end = .;
    }
    _dualport_sram_cached_data_size = _dualport_sram_cached_data_end - _dualport_sram_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_ucdata . :
#ifndef PRELOADED_INT_SRAM
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size + _dualport_sram_cached_data_size)
#else
        AT (ADDR(.dualport_sram_ucdata))
#endif
    {
        _dualport_sram_uncached_data_start = .;
        *(.dualport_sram_ucdata)
        . = ALIGN(0x10);
        _dualport_sram_uncached_data_end = .;
    }
    _dualport_sram_uncached_data_size = _dualport_sram_uncached_data_end - _dualport_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _dualport_sram_end =  .;
    _dualport_sram_size = _dualport_sram_end - _dualport_sram_start;

    /*----------------------- */
    /* BB DUALPORT SRAM BSS      */
    /*----------------------- */
    _dualport_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_ucbss . (NOLOAD) : AT (ADDR(.dualport_sram_ucbss))
    {
        *(.dualport_sram_ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_bss . (NOLOAD) : AT (ADDR(.dualport_sram_bss))
    {
        *(.dualport_sram_bss)
        . = ALIGN(0x10);
    }

    _dualport_bss_end = .;
    _dualport_bss_size = _dualport_bss_end - _dualport_bss_start;

    LD_ASSERT(ABSOLUTE(BB_DUALPORT_SRAM_BASE + BB_DUALPORT_SRAM_SIZE - RESERVED_MBX_SIZE) >= ABSOLUTE(_dualport_bss_end), "DPOV", "BB DUALPORT SRAM overflow.")

#endif /* (CHIP_ASIC_ID != 6) */

#ifndef PRELOADED_INT_SRAM
    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location + _dualport_sram_size;
#else
    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location;
#endif

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */

    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */

#ifdef RAMRUN
    /* For ramruns, this section loads an execution context
     * content allowing to directly executed the
     * boot sector (preloaded with PC=boot_Sector and SP=0,
     * cf boot_sector.c) */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) || (CHIP_ASIC_ID == 9)
    /* .sram_hst_monitor_ctx on gallite, 8808, and 8809 is loaded at cached address */
     _executor_ctx_address_offset = 0;
#else
    /* .sram_hst_monitor_ctx is loaded at uncached address */
     _executor_ctx_address_offset = MEM_CACHED_UNCACHED_OFFSET;
#endif
    .executor_ctx ROM_MISC_UBASE : AT (ADDR(.executor_ctx) - _executor_ctx_address_offset)
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
    }=0
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

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;

    _boot_sys_sram_start = .;

    _boot_sys_sram_txt_start = .;
#ifndef PRELOADED_INT_SRAM
    .bootsramtext . : AT (_boot_sys_sram_flash_start_location)
#else
    .bootsramtext . : AT (ADDR(.bootsramtext))
#endif
    {
        *(.bootsramtext)

        *drv_flsh.o(.sramtext)
        *drv_ddr.o(.sramtext)
        *hal_ebc.o(.sramtext)
        *hal_flash.o(.sramtext)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_txt_end = .;
    _boot_sys_sram_txt_size = _boot_sys_sram_txt_end - _boot_sys_sram_txt_start;

    _boot_sys_sram_data_start = .;
#ifndef PRELOADED_INT_SRAM
    .bootsramdata . : AT (_boot_sys_sram_flash_start_location + _boot_sys_sram_txt_size)
#else
    .bootsramdata . : AT (ADDR(.bootsramdata))
#endif
    {
        *(.bootsramdata)

        *drv_flsh.o(.sramdata)
        *drv_ddr.o(.sramdata)
        *hal_ebc.o(.sramdata)
        *hal_flash.o(.sramdata)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_data_end = .;
    _boot_sys_sram_data_size = _boot_sys_sram_data_end - _boot_sys_sram_data_start;

    _boot_sys_sram_end = .;
    _boot_sys_sram_size = _boot_sys_sram_end - _boot_sys_sram_start;

#ifndef PRELOADED_INT_SRAM
    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location + _boot_sys_sram_size;
#else
    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location;
#endif

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _boot_sys_sram_flash_end_location;

    _sys_sram_start = .;
    _sys_sram_cached_start = .;

#ifndef PRELOADED_INT_SRAM
    .sramtext . : AT (_sys_sram_flash_start_location)
#else
    .sramtext . : AT (ADDR(.sramtext))
#endif
    {
        _sys_sram_txt_start = .;

        *(.sramtext .sramrodata)

#ifdef RAMRUN
        _boot_sector = .;
        *(.boot_sector_start)

        *(.boottext)
        *(.bootrodata)

        *boot_sector.o(ROTEXT() RODATA())
        *boot_sector_chip.o(ROTEXT() RODATA())
        *boot_sys.o(ROTEXT() RODATA())
        *boot_mem_init.o(ROTEXT() RODATA())
        *boot_key.o(ROTEXT() RODATA())
        *boot_sector_driver.o(ROTEXT() RODATA())
        *target_boot_config.o(ROTEXT() RODATA())

        _code_base = .;
        *boot_loader.o(.text.boot_loader_start)
        *boot_loader.o(ROTEXT() RODATA())
#endif /* RAMRUN */

        *pal_gsmr.o(ROTEXT())
        *sxr_mem.o(ROTEXT())
        *sxr_jbsd.o(ROTEXT())
        *(.syssram_L1_text) 
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

    /* Overlay in SYS SRAM*/
    _sys_sram_overlay_start = .;
    OVERLAY . : NOCROSSREFS AT (_sys_sram_flash_end_location)
    {
        #include "int_sram_overlay.ld"
    }

    /* ld-2.15 End of overlay bug workaround (Manual calculation)*/
    _sys_sram_overlay_end           = _sys_sram_overlay_start + MAX_SIZE_OF_SYS_SRAM_OVERLAY;
    . = _sys_sram_overlay_end;

    _sys_sram_overlay_flash_size    = FLASH_SIZE_OF_SYS_SRAM_OVERLAY;

    _sys_sram_overlay_flash_end_location = _sys_sram_flash_end_location + _sys_sram_overlay_flash_size;

    /*----------------------- */
    /* SYS SRAM HEAP 1        */
    /*----------------------- */
    _sys_sram_heap_start = _sys_sram_overlay_end;

    _sys_sram_top = SYS_SRAM_END; /* End of internal sram */
    _sys_sram_heap_end = _sys_sram_top - (SYS_SRAM_IRQ_STACKWATCH_SIZE) - (XCPU_IRQ_STACK_SIZE);

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
    _boot_ram_flash_start_location = _fast_cpu_memory_overlay_flash_end_location;

#ifndef RAMRUN
    . = _ram_available_base;
#else /* RAMRUN */
    /* In a RAMRUN, all the flash content will be put in the ram, and
     * _ram_flash_start_location is in fact pointing to a ram location. */
    . = _boot_ram_flash_start_location;
#endif /* RAMRUN */


    .dsp_iq_data . : AT (ADDR(.dsp_iq_data))
    {
        *(.dsp_iq_data)
        . = ALIGN(0x10);
    }
        
    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _boot_ram_start = .;

    _boot_ram_txt_start = .;
#ifndef PRELOADED_RAM
    .bootramtext . : AT (_boot_ram_flash_start_location )
#else
    .bootramtext . : AT (ADDR(.bootramtext))
#endif
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootramtext)

        *hal_spi_flash.o(ROTEXT() RODATA())
        *drv_flsh.o(ROTEXT() RODATA())
        *drv_ddr.o(ROTEXT() RODATA())
        *drv_flsh_map.o(ROTEXT() RODATA())
        *hal_ebc.o(ROTEXT() RODATA())
        *hal_flash.o(ROTEXT() RODATA())

        *InfoNES.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
        *K6502.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
        *InfoNES_pAPU.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
#ifdef RAMRUN
		*(.romtext)
#endif
        . = ALIGN(0x10);
    }
    _boot_ram_txt_end = .;
    _boot_ram_txt_size = _boot_ram_txt_end - _boot_ram_txt_start;

    _boot_ram_cached_data_start = .;
#ifndef PRELOADED_RAM
    .bootdata . : AT (_boot_ram_flash_start_location + _boot_ram_txt_size)
#else
    .bootdata . : AT (ADDR(.bootdata))
#endif
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootdata)

        *hal_spi_flash.o(DATA())
        *drv_flsh.o(DATA())
        *hal_ebc.o(DATA())
        *hal_flash.o(DATA())
        . = ALIGN(0x10);
    }
    _boot_ram_cached_data_end = .;
    _boot_ram_cached_data_size = _boot_ram_cached_data_end - _boot_ram_cached_data_start;

    _boot_ram_end = .;
    _boot_ram_size = _boot_ram_end - _boot_ram_start;

#ifndef PRELOADED_RAM
    _boot_ram_flash_end_location =  _boot_ram_flash_start_location + _boot_ram_size;
#else
    _boot_ram_flash_end_location = _boot_ram_flash_start_location;
#endif

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _ram_flash_start_location = _boot_ram_flash_end_location;

    _ram_start = .;

    /* ------------------------------------------------ */
    /* Ext RAM CACHED  - SECTIONS ALWAYS IN RAM */
    /* ------------------------------------------------ */
#ifndef PRELOADED_RAM
    .ram . : AT (_ram_flash_start_location)
#else
    .ram . : AT (ADDR(.ram))
#endif
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
#ifndef PRELOADED_RAM
    .ramtext _ram_special_cached_end : AT (_ram_flash_start_location + _ram_special_cached_size)
#else
    .ramtext _ram_special_cached_end : AT (ADDR(.ramtext))
#endif /*PRELOADED_RAM*/
    {
        _ram_cached_txt_start = .;
#ifdef CODE_IN_RAM
        *(.text .text.* .stub .gnu.linkonce.t.*)
#endif

#ifdef PROTOSTACK_IN_RAM
        #include "section_stack_gprs.ld"
#else
  *sxr_mem.o(.text .text.* .rodata .rodata.*)
  *sxr_usrmem.o(.text .text.* .rodata .rodata.*)
  *sxr_jbsd.o(.text .text.* .rodata .rodata.*)
  *sxr_sbx.o(.text .text.* .rodata .rodata.*)
  *sxr_tim.o(.text .text.* .rodata .rodata.*)
  *sxs_cksum.o(.text .text.* .rodata .rodata.*)
   *hal_irq_handler.o(.text .text.* .rodata .rodata.*)
#endif

#if !defined(COMPRESS_PROCESS_ENABLE)
	*vpp_audio_dec_tab_asm.o(.text .text.* .rodata .rodata.*)
	*resource_audio.o(.text .text.* .rodata .rodata.*)
	*resource_audio_app.o(.text .text.* .rodata .rodata.*)
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

    /*---------------------- */
    /* Ext RAM UNCACHED Data */
    /*---------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

#ifndef PRELOADED_RAM
    .ucdata . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size)
#else
    .ucdata . : AT (ADDR(.ucdata))
#endif
    {
        _ram_uncached_data_start = .;
        *(.ucdata)
        *(*.globalvars.uc)
        . = ALIGN(0x20);
    }
    _ram_uncached_data_end = .;
    _ram_uncached_data_size = _ram_uncached_data_end - _ram_uncached_data_start;

    /*---------------------- */
    /* Ext RAM CACHED Data */
    /*---------------------- */
    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;
#ifndef PRELOADED_RAM
    .data . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size + _ram_uncached_data_size)
#else
    .data . : AT (ADDR(.data))
#endif
    {
        _ram_cached_data_start = . ;

        /* NOTE: globalvars.c sections are to be removed.
         * They are kept here for backward compatibility of SVC/VPP codes.
         * These codes are linked from Total_BJ branch and the special global
         * variable sections will be removed in the future. */
        *(*.globalvars.c *.globalvars.c.*)

        *( .data)
        *( .data.*)
        *( .gnu.linkonce.d.*)
        *(.data1)
        _gp = ALIGN(16) + 0x7ff0;
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.lit8)
        *(.lit4)
        . = ALIGN(0x4);
        /***barrier : do not remove or edit */
        . = ALIGN(0x20);
    }
    _ram_cached_data_end = . ;
    _ram_cached_data_size = _ram_cached_data_end - _ram_cached_data_start;
    _ram_end = .;
    _ram_size = _ram_end - _ram_start;

#ifndef PRELOADED_RAM
    _ram_flash_end_location =  _ram_flash_start_location + _ram_size;
#else
    _ram_flash_end_location = _ram_flash_start_location;
#endif

    /*=================================================== */
    /* Code sections to be compressed (link stage 1)      */
    /*=================================================== */
#if (defined(COMPRESS_PROCESS_ENABLE) && defined(CT_COMPRESS_CODE_SECTION))

    LD_ASSERT(RAM_PHY_SIZE >= RAM_SIZE, "RAMS", "Invalid RAM size configuration. Please check RAM_PHY_SIZE and RAM_SIZE in memd.def.")

#ifdef COMPRESS_3264
    #include "compress_code_section_3264.ld"
#else
#ifdef COMPRESS_6464
    #include "compress_code_section_6464.ld"
#else
#ifdef COMPRESS_3232
    #include "compress_code_section_3232.ld"
#else
#ifdef COMPRESS_3232_NO_OVERLAY
    #include "compress_code_section_3232_no_overlay.ld"
#else
#ifdef COMPRESS_QVGA3232
    #include "compress_code_section_qvga_3232.ld"
#else
#ifdef COMPRESS_SLIMPDA6464
    #include "compress_code_section_slimpda_6464.ld"
#else
    #include "compress_code_section.ld"
#endif	/* SLIMPDA6464 */
#endif	/* QVGA3232 */
#endif/* 3232_no_overlay*/
#endif /* 3232 */
#endif /* 6464 */
#endif /* 3264 */

#endif /* COMPRESS_PROCESS_ENABLE && CT_COMPRESS_CODE_SECTION */

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

    /* ------------------------------------------------ */
    /* BOOTING BSS (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_ram_bss_start = .;
    .bootbss . (NOLOAD) : AT (ADDR(.bootbss))
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers.*/
        *(.bootbss)

        *hal_spi_flash.o(BSS())
        *drv_flsh.o(BSS())
        *hal_ebc.o(BSS())
        *hal_flash.o(BSS())
        . = ALIGN(0x10);
    }
    _boot_ram_bss_end = .;
    _boot_ram_bss_size = _boot_ram_bss_end - _boot_ram_bss_start;

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
    _ram_top = EXT_RAM_BASE + RAM_SIZE;
    _heap_size = _ram_top -_ram_bss_end;
    _heap_start =  _ram_bss_end;
    _heap_end =  _ram_bss_end + _heap_size;

    /* Check that the external RAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(_ram_top) > ABSOLUTE(_ram_bss_end), "EXTS", "External RAM overflow !")


    /*========================================================= */
    /* In FLASH                                                 */
    /*========================================================= */
    _rom_flash_start_location = _ram_flash_end_location;
    . = _rom_flash_start_location;

    /* .crossrefs section can be used to list functions which is share by other application */
    .crossrefs . : AT (ADDR(.crossrefs))
    {
        *(.crossref)
        . = ALIGN(0x10);
    }

#ifndef COMPRESS_PROCESS_ENABLE
    .init . : AT(ADDR(.init))
    {
        *(.init)
        . = ALIGN(0x10);
    }
#endif

    .rotext . : AT (ADDR(.rotext))
    {
#ifndef RAMRUN
        *(.romtext)
#endif
#ifndef CODE_IN_RAM
        *(.text)
        *(.text.*)
        *(.stub .gnu.linkonce.t.*)
#endif
        . = ALIGN(0x20);
    }

    .rodata . : AT (ADDR(.rodata))
    {
#ifndef CONST_IN_RAM
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
        KEEP(*(.dbgfunc))
         . = ALIGN(0x10);
    }

    _flash_end = .;

    /* kept for compatibility */
    _etext = _ram_txt_end;
    _esramtext = _sys_sram_txt_end;
    _end = _ram_bss_end;


    /*=================================================== */
    /* Stub section to get compressed code & data         */
    /*=================================================== */
#ifdef COMPRESS_PROCESS_ENABLE
    _get_compressed_data_crc_start = .;
    .compressed_data.crc . : AT (ADDR(.compressed_data.crc))
    {
      xcpu_overlay_init_func_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_2_section_1_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_2_section_2_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_2_section_3_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_3_section_1_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_3_section_2_crc = .;
      LONG(0)
      . = ALIGN(0x10);
      overlay_3_section_3_crc = .;
      LONG(0)
      . = ALIGN(0x10);
	KEEP(*(.compressed_data.crc))
    }
    _get_compressed_data_crc_end = .;

    _get_compressed_data_start = .;

    .get_compressed_data . : AT (ADDR(.get_compressed_data))
    {
        *get_compressed_data.o(.get_compressed_data)

#ifdef COMPRESS_PREMAIN
        premain_start = 0;
        premain_end = 0;
#endif

        init_func_start = 0;
        init_func_end = 0;
        csdapp_start = 0;
        csdapp_end = 0;
        wap_start = 0;
        wap_end = 0;
        other_start = 0;
        other_end = 0;
        overlay_2_section_1_start = 0;
        overlay_2_section_1_end = 0;
        overlay_2_section_2_start = 0;
        overlay_2_section_2_end = 0;
        overlay_2_section_3_start = 0;
        overlay_2_section_3_end = 0;
        overlay_3_section_1_start = 0;
        overlay_3_section_1_end = 0;
        overlay_3_section_2_start = 0;
        overlay_3_section_2_end = 0;
        overlay_3_section_3_start = 0;
        overlay_3_section_3_end = 0;
        overlay_4_section_1_start = 0;
        overlay_4_section_1_end = 0;
        overlay_4_section_2_start = 0;
        overlay_4_section_2_end = 0;
        . = ALIGN(0x10);
    }
    _get_compressed_data_end  = .;


#endif /* COMPRESS_PROCESS_ENABLE */

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

#else /* COMPRESS_PROCESS_ENABLE && LINK_STAGE_2 */

    #include "link_stage1_elf.ld"

    _code_base = ADDR(.main_entry_section);

    . = _get_compressed_data_crc_start;
    .compressed_data.crc . : AT (ADDR(.compressed_data.crc))
    {
        xcpu_overlay_init_func_crc = .;
        *xcpu_overlay_init_func.sum.o(.data .data.*)
        LONG(0)
        . = ALIGN(0x10);
        overlay_2_section_1_crc = .;
        *overlay_2_section_1.sum.o(.data .data.*)
        LONG(0)
        . = ALIGN(0x10);
        overlay_2_section_2_crc = .;
        *overlay_2_section_2.sum.o(.data .data.*)
        LONG(0)
       . = ALIGN(0x10);
        overlay_2_section_3_crc = .;
        *overlay_2_section_3.sum.o(.data .data.*)
        LONG(0)
       . = ALIGN(0x10);
        overlay_3_section_1_crc = .;
        *overlay_3_section_1.sum.o(.data .data.*)
        LONG(0)
       . = ALIGN(0x10);
        overlay_3_section_2_crc = .;
        *overlay_3_section_2.sum.o(.data .data.*)
        LONG(0)
       . = ALIGN(0x10);
        overlay_3_section_3_crc = .;
        *overlay_3_section_3.sum.o(.data .data.*)
        LONG(0)
        . = ALIGN(0x10);
      KEEP(*(.compressed_data.crc))
    }
    _get_compressed_data_crc_end = .;

    /* _get_compressed_data_start is derived from file ... */
    . = _get_compressed_data_start;
    .get_compressed_data . : AT (ADDR(.get_compressed_data))
    {
        *get_compressed_data.o(.get_compressed_data)

#ifdef COMPRESS_PREMAIN
        . = ALIGN(0x20);
        premain_start = .;
        *xcpu_premain.bin.lzma.o(.data .data.*)
        premain_end = .;
#endif

        . = ALIGN(0x20);
        init_func_start = .;
        *xcpu_overlay_init_func.bin.lzma.o(.data .data.*)
        init_func_end = .;
        . = ALIGN(0x20);
        overlay_2_section_1_start = .;
        *overlay_2_section_1.bin.lzma.o(.data .data.*)
        overlay_2_section_1_end = .;
        . = ALIGN(0x20);
        overlay_2_section_2_start = .;
        *overlay_2_section_2.bin.lzma.o(.data .data.*)
        overlay_2_section_2_end = .;
        . = ALIGN(0x20);
        overlay_2_section_3_start = .;
        *overlay_2_section_3.bin.lzma.o(.data .data.*)
        overlay_2_section_3_end = .;
        . = ALIGN(0x20);
        overlay_3_section_1_start = .;
        *overlay_3_section_1.bin.lzma.o(.data .data.*)
        overlay_3_section_1_end = .;
        . = ALIGN(0x20);
        overlay_3_section_2_start = .;
        *overlay_3_section_2.bin.lzma.o(.data .data.*)
        overlay_3_section_2_end = .;
        . = ALIGN(0x20);
        overlay_3_section_3_start = .;
        *overlay_3_section_3.bin.lzma.o(.data .data.*)
        overlay_3_section_3_end = .;
        . = ALIGN(0x20);
        overlay_4_section_1_start = .;
        *overlay_4_section_1.bin.lzma.o(.data .data.*)
        overlay_4_section_1_end = .;
        . = ALIGN(0x20);
        overlay_4_section_2_start = .;
        *overlay_4_section_2.bin.lzma.o(.data .data.*)
        overlay_4_section_2_end = .;
        . = ALIGN(0x20);
    }

    _get_compressed_data_end  = .;

    _flash_end = .;

#endif /* COMPRESS_PROCESS_ENABLE && LINK_STAGE_2 */

    /* Check that the data and code in flash do not overwrite */
    LD_ASSERT(ABSOLUTE(_flash_end) <= CODE_FLASH_PHY_END,
              "FLAS",
              "Flash overflow: there is no more space available in flash memory.")

    /*================================================= */
    /* Define user data boundaries based on the defines */
    /*================================================= */
    .user_data USER_DATA_PHY_BASE : AT (ADDR(.user_data))
    {
        _user_data_start = .;
#if !(defined(COMPRESS_PROCESS_ENABLE) && !defined(LINK_STAGE_2))
        *user.bin.o(.user_data);
        KEEP(*(.user_data))
#endif
    }

    _user_data_end = .;
    _user_data_size = _user_data_end - _user_data_start;


#ifdef REDUNDANT_DATA_REGION

    /*================================================= */
    /* Define remain data boundaries based on the defines */
    /*================================================= */

    .remain_data REMAIN_BASE : AT (ADDR(.remain_data))
    {
        _remain_start = .;
        KEEP(*(.remain_data))
    }
    _remain_end = _remain_start + REMAIN_SIZE;

#endif
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
#ifdef REDUNDANT_DATA_REGION

    LD_ASSERT(ABSOLUTE(_calib_start) >= ABSOLUTE (_remain_end),
              "FLAS",
              "Remain data is overflow.")

     LD_ASSERT(ABSOLUTE(_remain_end - _remain_start) >= 0x1000,
              "FLAS",
              "Remain sector is null.")

#endif
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
     * 1) Code (including boot code) -> User Data -> Calib Data -> Factory Data
     * 2) Boot Sector -> Calib Data -> Factory Data -> Code (excluding boot code) -> User Data
     * /

    /*Check that the user data in flash do not overwrite
     * the calib data or factory setting space */
    LD_ASSERT(ABSOLUTE(_calib_start) >= ABSOLUTE(_user_data_end) ||
              ABSOLUTE(_factory_start) > ABSOLUTE(_code_base),
              "CFOV", "Calib data is overlapped with user data or factory data is overlapped with code.")

    /*Check that the calib data in flash do not overwrite
     * the factory setting data space */
    LD_ASSERT(ABSOLUTE(_factory_start) > ABSOLUTE(_calib_start),
              "FACO", "Factory setting data is overlapped with calib data.")


    #include "discard.ld"
}

#ifdef COMPRESS_DATA_SECTION_NOCROSSREFS
COMPRESS_DATA_SECTION_NOCROSSREFS
#endif

