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


ENTRY (boot_Sector)
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

    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */

    . = EXT_RAM_BASE;

    _ram_available_base = .;

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
#ifdef ROMULATOR
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

    . = FLASH_BASE;

#endif /* NAND_FLASH != 1 */

    _flash_start = .;
    /* ============================= */
    /* boot sector in flash          */
    /* ============================= */
    .boot_code _flash_start : AT (ADDR(.boot_code))
    {
#ifdef ROMULATOR
        LONG(0xD9EF0045)
#else
        LONG(0xFFFFFFFF)
#endif /* ROMULATOR */

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

        . = ALIGN(0x10);
    }

    /* ============================== */
    /* Main entry point after monitor */
    /* ============================== */

#if (CODE_BASE != FLASH_BASE)
    /* ========================================== */
    /* set current location counter to code base if it differs from flash base */
    /* ========================================== */
    LD_ASSERT(ABSOLUTE(CODE_BASE) >= ABSOLUTE(.), "CBE", "CODE_BASE is invalid. \
Note that CODE_BASE is designed to be used only when calib and factory sectors are located at flash forepart, \
and it should never be set in all other cases.")

    . = CODE_BASE;

#endif /* CODE_BASE != FLASH_BASE */

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

    /* ===================================================== */
    /* in BB DUALPORT SRAM                                      */
    /* ===================================================== */
#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 8) || \
    (CHIP_ASIC_ID == 10) /* gallite or 8808 or 8810 */
    _dualport_sram_flash_start_location = _bb_sram_flash_end_location;
#else
    _dualport_sram_flash_start_location = _int_rom_flash_end_location;
#endif

#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 10) /* gallite or 8810 */

    _dualport_sram_size = 0;

#else /* !gallite && !8810 */

#if (NAND_FLASH == 1)
    . = BB_DUALPORT_SRAM_BASE + NAND_FLASH_BOOT_CODE_SIZE;
#else
    . = BB_DUALPORT_SRAM_BASE;
#endif

    _dualport_sram_start = .;

    .dualport_sram_text . :
        AT (_dualport_sram_flash_start_location)
    {
        _dualport_sram_text_start = .;
        . = ALIGN(0x10);
        _dualport_sram_text_end = .;
    }
    _dualport_sram_text_size = _dualport_sram_text_end - _dualport_sram_text_start;

    .dualport_sram_data . :
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size)
    {
        _dualport_sram_cached_data_start = .;
        . = ALIGN(0x10);
        _dualport_sram_cached_data_end = .;
    }
    _dualport_sram_cached_data_size = _dualport_sram_cached_data_end - _dualport_sram_cached_data_start;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_ucdata . :
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size + _dualport_sram_cached_data_size)
    {
        _dualport_sram_uncached_data_start = .;
        . = ALIGN(0x10);
        _dualport_sram_uncached_data_end = .;
    }
    _dualport_sram_uncached_data_size = _dualport_sram_uncached_data_end - _dualport_sram_uncached_data_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _dualport_sram_end =  .;
    _dualport_sram_size = _dualport_sram_end - _dualport_sram_start;

    /*----------------------- */
    /* DUALPORT SRAM BSS      */
    /*----------------------- */
    _dualport_bss_start = .;

    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_ucbss . (NOLOAD) : AT (ADDR(.dualport_sram_ucbss))
    {
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .dualport_sram_bss . (NOLOAD) : AT (ADDR(.dualport_sram_bss))
    {
        . = ALIGN(0x10);
    }

    _dualport_bss_end = .;
    _dualport_bss_size = _dualport_bss_end - _dualport_bss_start;

    LD_ASSERT(ABSOLUTE(BB_DUALPORT_SRAM_BASE + BB_DUALPORT_SRAM_SIZE - RESERVED_MBX_SIZE) >= ABSOLUTE(_dualport_bss_end), "DPOV", "BB DUALPORT SRAM overflow.")

#endif /* !gallite && !8810 */

    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location + _dualport_sram_size;

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */

    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */

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
    .irqsram . : AT (_irq_sys_sram_flash_start_location)
    {
        . = ALIGN(0x10);
    }
    _irq_sys_sram_end = .;
    _irq_sys_sram_size = _irq_sys_sram_end - _irq_sys_sram_start;

    _irq_sys_sram_flash_end_location = _irq_sys_sram_flash_start_location + _irq_sys_sram_size;

    /* ------------------------------------------------ */
    /* BOOT SYS SRAM - BOOTING SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;

    _boot_sys_sram_start = .;

    _boot_sys_sram_txt_start = .;
    .bootsramtext . : AT (_boot_sys_sram_flash_start_location)
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
    .bootsramdata . : AT (_boot_sys_sram_flash_start_location + _boot_sys_sram_txt_size)
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

    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location + _boot_sys_sram_size;

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _boot_sys_sram_flash_end_location;

    _sys_sram_start = .;
    _sys_sram_cached_start = .;

    .sramtext . : AT (_sys_sram_flash_start_location)
    {
        _sys_sram_txt_start = .;
        . = ALIGN(0x10);
    }
    _sys_sram_txt_end = .;
    _sys_sram_txt_size = _sys_sram_txt_end - _sys_sram_start;


    .sramdata _sys_sram_txt_end : AT (_sys_sram_flash_start_location + _sys_sram_txt_size )
    {
        _sys_sram_data_start = .;
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

    .sramucdata . : AT (_sys_sram_flash_start_location + _sys_sram_cached_size)
    {
        _sys_sram_uncached_start = .;
        . = ALIGN(0x10);
    }
    _sys_sram_uncached_end = .;
    _sys_sram_uncached_size = _sys_sram_uncached_end - _sys_sram_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _sys_sram_end =  .;
    _sys_sram_size = _sys_sram_end - _sys_sram_start;

    _sys_sram_flash_end_location = _sys_sram_flash_start_location + _sys_sram_size;

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
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .srambss . (NOLOAD) : AT (ADDR(.srambss))
    {
        . = ALIGN(0x10);
    }

    _sys_sram_bss_end = .;
    _sys_sram_bss_size = _sys_sram_bss_end - _sys_sram_bss_start;

    /* Overlay in SYS SRAM*/
    _sys_sram_overlay_start = .;

    /* ld-2.15 End of overlay bug workaround (Manual calculation)*/
    _sys_sram_overlay_end           = _sys_sram_overlay_start;
    . = _sys_sram_overlay_end;

    _sys_sram_overlay_flash_size    = 0;

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

    _fast_cpu_memory_overlay_flash_end_location = _sys_sram_overlay_flash_end_location;


    /* ========================================== */
    /* continued in External RAM                  */
    /* ========================================== */
    _boot_ram_flash_start_location = _fast_cpu_memory_overlay_flash_end_location;

    . = _ram_available_base;

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _boot_ram_start = .;

    _boot_ram_txt_start = .;
    .bootramtext . : AT (_boot_ram_flash_start_location )
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootramtext)

        *drv_flsh.o(ROTEXT() RODATA())
		*drv_ddr.o(ROTEXT() RODATA())
        *drv_flsh_map.o(ROTEXT() RODATA())
        *hal_ebc.o(ROTEXT() RODATA())
        *hal_flash.o(ROTEXT() RODATA())
        . = ALIGN(0x10);
    }
    _boot_ram_txt_end = .;
    _boot_ram_txt_size = _boot_ram_txt_end - _boot_ram_txt_start;

    _boot_ram_cached_data_start = .;
    .bootdata . : AT (_boot_ram_flash_start_location + _boot_ram_txt_size)
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootdata)

        *drv_flsh.o(DATA())
        *hal_ebc.o(DATA())
        *hal_flash.o(DATA())
        . = ALIGN(0x10);
    }
    _boot_ram_cached_data_end = .;
    _boot_ram_cached_data_size = _boot_ram_cached_data_end - _boot_ram_cached_data_start;

    _boot_ram_end = .;
    _boot_ram_size = _boot_ram_end - _boot_ram_start;

    _boot_ram_flash_end_location =  _boot_ram_flash_start_location + _boot_ram_size;

    /* ------------------------------------------------ */
    /* NORMAL RAM - NON-BOOTING SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _ram_flash_start_location = _boot_ram_flash_end_location;

    _ram_start = .;

    /* ------------------------------------------------ */
    /* Ext RAM CACHED  - SECTIONS ALWAYS IN RAM */
    /* ------------------------------------------------ */
    .ram . : AT (_ram_flash_start_location)
    {
        _ram_special_cached_start = .;
        _ram_txt_start = .;
        . = ALIGN(0x10);
    }
    _ram_special_cached_end = .;
    _ram_special_cached_size = _ram_special_cached_end - _ram_special_cached_start;

    /*---------------------------- */
    /* Ext RAM CACHED TXT          */
    /*---------------------------- */
    .ramtext _ram_special_cached_end : AT (_ram_flash_start_location + _ram_special_cached_size)
    {
        _ram_cached_txt_start = .;
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

    .ucdata . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size)
    {
        _ram_uncached_data_start = .;
        . = ALIGN(0x10);
    }
    _ram_uncached_data_end = .;
    _ram_uncached_data_size = _ram_uncached_data_end - _ram_uncached_data_start;

    /*---------------------- */
    /* Ext RAM CACHED Data */
    /*---------------------- */
    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;
    .data . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size + _ram_uncached_data_size)
    {
        _ram_cached_data_start = . ;
        . = ALIGN(0x10);
    }
    _ram_cached_data_end = . ;
    _ram_cached_data_size = _ram_cached_data_end - _ram_cached_data_start;
    _ram_end = .;
    _ram_size = _ram_end - _ram_start;

    _ram_flash_end_location =  _ram_flash_start_location + _ram_size;

    /*-------------------------------------------------------  */
    /* Uninitialized section in RAM (content kept after reboot)*/
    /*-------------------------------------------------------  */
    .backup . (NOLOAD) : AT (ADDR(.backup))
    {
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
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bss . (NOLOAD) : AT (ADDR(.bss))
    {
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

    .rotext . : AT (ADDR(.rotext))
    {
        *(.text)
        *(.text.*)
        *(.stub .gnu.linkonce.t.*)
        . = ALIGN(0x20);
    }

    .rodata . : AT (ADDR(.rodata))
    {
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


    /*================================================= */
    /* Define user data boundaries based on the defines */
    /*================================================= */
    .user_data USER_DATA_BASE : AT (ADDR(.user_data))
    {
        _user_data_start = .;
    }
    _user_data_end = .;
    _user_data_size = _user_data_end - _user_data_start;

    /*Check that the data and code in flash do not overwrite
     * the user data space */
    /* ASSERT seems to be boguous as of binutils-2.15 : we are
     * not allowed to used error messages longer than FLAS in that
     * particular case. Don't ask and port a newer version,
     * maybe. FIXME */
    LD_ASSERT(ABSOLUTE(_user_data_start) > ABSOLUTE(_flash_end),
              "FLAS",
              "Flash overflow: there is no more space available in flash memory.")


    /* ============================== */
    /* calib sector in flash          */
    /* ============================== */

    .calib CALIB_BASE : AT (ADDR(.calib))
    {
        _calib_start = .;
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

