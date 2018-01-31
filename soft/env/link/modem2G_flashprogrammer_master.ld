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
/* This linker script is dedicated to the flash_programmer.
 * The linker script depends on the following defines */
/*  - CONST_IN_RAM : all constants are placed in SRAMrun */
/*  - CODE_IN_RAM : All code is executed from SRAM, this is useful for ram_run or code running from RAM on romulator */
/*  - PRELOADED_INT_SRAM : We fill it with a ramrun, so this is always true. */
/*  - PRELOADED_RAM : We don't use any external ram */
/*  - RAMRUN :  The flashprogrammer is always ramruned */

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

    _bb_sram_flash_start_location = . ;

    _bb_sram_flash_end_location  = _bb_sram_flash_start_location;

    /* ===================================================== */
    /* in BB DUALPORT SRAM                                      */
    /* ===================================================== */
    _dualport_sram_flash_start_location = _bb_sram_flash_end_location;

#if (CHIP_ASIC_ID == 6) || (CHIP_ASIC_ID == 10)|| (CHIP_ASIC_ID == 16)  /* gallite or 8810 */

    _dualport_sram_size = 0;

#else /* !gallite && !8810 */

    /* NAND_FLASH: No need to reserve a nand boot code space for flash programmer */
    . = BB_DUALPORT_SRAM_BASE;

    _dualport_sram_start = .;

    .dualport_sram_text . :
        AT (ADDR(.dualport_sram_text))
    {
        _dualport_sram_text_start = .;
        *(.dualport_sram_text)
        *(.dualport_sram_rodata)
        . = ALIGN(0x10);
        _dualport_sram_text_end = .;
    }
    _dualport_sram_text_size = _dualport_sram_text_end - _dualport_sram_text_start;

    .dualport_sram_data . :
        AT (ADDR(.dualport_sram_data))
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
        AT (ADDR(.dualport_sram_ucdata))
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

#endif /* !gallite && !8810 */

    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location;

    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */

    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */

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
    .irqsram . : AT (ADDR(.irqsram))
    {
        . = ALIGN(0x10);
    }
    _irq_sys_sram_end = .;
    _irq_sys_sram_size = _irq_sys_sram_end - _irq_sys_sram_start;

    _irq_sys_sram_flash_end_location = _irq_sys_sram_flash_start_location;

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;

    _boot_sys_sram_start = .;

    _boot_sys_sram_txt_start = .;
    .bootsramtext . : AT (ADDR(.bootsramtext))
    {
        . = ALIGN(0x10);
    }
    _boot_sys_sram_txt_end = .;
    _boot_sys_sram_txt_size = _boot_sys_sram_txt_end - _boot_sys_sram_txt_start;

    _boot_sys_sram_data_start = .;
    .bootsramdata . : AT (ADDR(.bootsramdata))
    {
        . = ALIGN(0x10);
    }
    _boot_sys_sram_data_end = .;
    _boot_sys_sram_data_size = _boot_sys_sram_data_end - _boot_sys_sram_data_start;

    _boot_sys_sram_end = .;
    _boot_sys_sram_size = _boot_sys_sram_end - _boot_sys_sram_start;

    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location;

    /* Stubbed symbols for modem codes */
    _boot_sys_sram_data_load_start = .;
    _boot_sys_sram_data_load_end = _boot_sys_sram_data_load_start;

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _boot_sys_sram_flash_end_location;

    _sys_sram_start = .;
    _sys_sram_cached_start = .;

    .sramtext . : AT (ADDR(.sramtext))
    {
        _sys_sram_txt_start = .;

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

        *(.bootsramtext)
        *(.bootramtext)
        *(.sramtext .sramrodata)
        *(.ram)
        *(.text .text.*)
        *(.romtext)

        . = ALIGN(0x10);
    }
    _sys_sram_txt_end = .;
    _sys_sram_txt_size = _sys_sram_txt_end - _sys_sram_start;

    .sramdata _sys_sram_txt_end : AT (ADDR(.sramdata))
    {
        _sys_sram_data_start = .;
        *(.bootsramdata)
        *(.bootdata)
        *(.sramdata)
        *(.srrodata)
        *(.rodata.*)
        *(.rodata)
        *(.sdata2.*)
        *(.sdata2)
        *(.sdata.*)
        *(.sdata)
        *(.data.*)
        *(.data)
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
    .sramuctext . : AT (ADDR(.sramuctext))
    {
    	_sys_sram_uncached_txt_start = .;
    	*(.sramuctext)
    }
    _sys_sram_uncached_txt_end = .;
    _sys_sram_uncached_txt_size = _sys_sram_uncached_txt_end - _sys_sram_uncached_txt_start;

    .sramucdata . : AT (ADDR(.sramucdata))
    {
        _sys_sram_uncached_start = .;
        *(.sramucdata)
        *(.srroucdata)
        *(.ucdata)
        *(.backup)
        *(.ucbackup)
        /* FIXME We don't know what this is for but
         * we put it there for now. */
        *(.rel.dyn)
        . = ALIGN(0x10);
    }
    _sys_sram_uncached_end = .;
    _sys_sram_uncached_size = _sys_sram_uncached_end - _sys_sram_uncached_start;

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    _sys_sram_end =  .;
    _sys_sram_size = _sys_sram_end - _sys_sram_start;

    _sys_sram_flash_end_location = _sys_sram_flash_start_location;

    /* ------------------------------------------------ */
    /* BOOTING BSS IN SYS SRAM (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_sys_sram_bss_start = .;
    .bootsrambss . (NOLOAD) : AT (ADDR(.bootsrambss))
    {
        *(.bootsrambss)
        *(.bootbss)
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
        *(.ucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .srambss . (NOLOAD) : AT (ADDR(.srambss))
    {
        *(.srambss)
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(0x10);
    }

    _sys_sram_bss_end = .;
    _sys_sram_bss_size = _sys_sram_bss_end - _sys_sram_bss_start;

    _sys_sram_top = SYS_SRAM_END; /* End of internal sram */
    /* Check that the INT SRAM is not over stuffed */
    LD_ASSERT(ABSOLUTE(_sys_sram_top) >= ABSOLUTE(_sys_sram_bss_end), "INTS", "Internal SRAM overflow !")

    /* ============================== */
    /* calib sector in sram            */
    /* ============================== */
    .calib . : AT (ADDR(.calib))
    {
        _calib_start = .;
        *(.calib);
        . = ALIGN(0x10);
    }
    _calib_end = .;
    _calib_size = _calib_end - _calib_start;

    /* ============================== */
    /* factory settings in sram      */
    /* ============================== */
    .factory_settings . : AT (ADDR(.factory_settings))
    {
        _factory_start = .;
        *(.factory_settings)
        . = ALIGN(0x10);
    }
    _factory_end = .;
    _factory_size = _factory_end - _factory_start;

    /* ============================== */
    /* Main entry point after monitor */
    /* ============================== */
    .main_entry_section . : AT (ADDR(.main_entry_section))
    {
        _board_config_start = .;
        *(.board_config)
        . = ALIGN(0x10);
        _board_config_end = .;
        . = ALIGN(0x10);
    }

    _board_config_size = _board_config_end - _board_config_start;

    /*----------------------- */
    /* SYS SRAM HEAP 1        */
    /*----------------------- */
    _sys_sram_top = SYS_SRAM_END; /* End of internal sram */
    _sys_sram_heap_start = _board_config_end;
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


    /*---------------- */
    /* End of SYS SRAM */
    /*---------------- */

#if (CHIP_HAS_PROXY_MEMORY == 1)
    /* ========================================== */
    /* Proxy Memory                               */
    /* ========================================== */
    /* Standard usage of proxy memory */
    /* Manual placing and copy from flash */
    /* FIXME This section will probably not remain like this,
     * so check it's up-to-date. */
    _proxy_memory_flash_start_location = _sys_sram_flash_end_location;
    .proxy_memory SYS_PROXY_MEM_BASE : AT (ADDR(.proxy_memory))
    {
        _proxy_memory_start = .;
/* Loadable as a ramrun since the proxy memory is enabled
   by the romed monitor.*/
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

    _proxy_memory_flash_end_location = _proxy_memory_flash_start_location;
    _proxy_memory_top = SYS_PROXY_MEM_BASE + SYS_PROXY_MEM_SIZE - 4;
	/* FIXME Add a check about the IRQ reserved size and the no
	overwrite of the code section of this proxy memory */
#else // CHIP_HAS_PROXY_MEMORY
    _proxy_memory_flash_end_location = _sys_sram_flash_end_location;
#endif // CHIP_HAS_PROXY_MEMORY


    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */
    _boot_ram_flash_start_location = _proxy_memory_flash_end_location;

    . = EXT_RAM_BASE;

    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _boot_ram_start = .;

    _boot_ram_txt_start = .;
    .bootramtext . : AT (ADDR(.bootramtext))
    {
        . = ALIGN(0x10);
    }
    _boot_ram_txt_end = .;
    _boot_ram_txt_size = _boot_ram_txt_end - _boot_ram_txt_start;

    _boot_ram_cached_data_start = .;
    .bootdata . : AT (ADDR(.bootdata))
    {
        . = ALIGN(0x10);
    }
    _boot_ram_cached_data_end = .;
    _boot_ram_cached_data_size = _boot_ram_cached_data_end - _boot_ram_cached_data_start;

    _boot_ram_end = .;
    _boot_ram_size = _boot_ram_end - _boot_ram_start;

    _boot_ram_flash_end_location = _boot_ram_flash_start_location;

    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _ram_flash_start_location = _boot_ram_flash_end_location;

    _ram_start = .;

    /* ------------------------------------------------ */
    /* Ext RAM CACHED  - SECTIONS ALWAYS IN RAM */
    /* ------------------------------------------------ */
    .ram . : AT (ADDR(.ram))
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
    .text  _ram_special_cached_end : AT (ADDR(.text))
    {
        _ram_cached_txt_start = . ;
        . = ALIGN(0x10);
    } = 0
    _ram_cached_txt_end = . ;
    ram_cached_txt_size = _ram_cached_txt_end - _ram_cached_txt_start;

    _ram_txt_end = .;

    /*---------------------- */
    /* Ext RAM UNCACHED Data */
    /*---------------------- */
    /* Go Uncached */
    . += MEM_CACHED_UNCACHED_OFFSET;

    .ucdata . : AT (ADDR(.ucdata))
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

    .data . : AT (ADDR(.data))
    {
        _ram_cached_data_start = . ;
        . = ALIGN(0x10);
    }
    _ram_cached_data_end = . ;
    _ram_cached_data_size = _ram_cached_data_end - _ram_cached_data_start;
    _ram_end = .;
    _ram_size = _ram_end - _ram_start;

    _ram_flash_end_location = _ram_flash_start_location;

    /* Stubbed symbols for modem codes */
    _ram_data_load_start = .;
    _ram_data_load_end = _ram_data_load_start;
    _ram_data_start = .;
    _ram_data_end = _ram_data_start;

    /* ------------------------------------------------ */
    /* BOOTING BSS (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_ram_bss_start = .;
    .bootbss . (NOLOAD) : AT (ADDR(.bootbss))
    {
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

    .ucbss    . (NOLOAD) : AT (ADDR(.ucbss))
    {
        *(.ramucbss)
        . = ALIGN(0x10);
    }

    /* Go Cached */
    . -= MEM_CACHED_UNCACHED_OFFSET;

    .bss . (NOLOAD) : AT (ADDR(.bss))
    {
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


    /* kept for compatibility */
    _etext = _ram_txt_end;
    _esramtext = _sys_sram_txt_end;
    _end = _ram_bss_end;


    /*=================================================== */
    /* Placeholder for misc hacks, not used for real apps */
    /*=================================================== */
    .misc . : AT (ADDR(.misc))
    {
        *(.misc)
        . = ALIGN(0x10);
    }


    #include "discard.ld"
}

